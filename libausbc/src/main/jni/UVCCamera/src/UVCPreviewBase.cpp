/*
 * UVCCamera
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014-2017 saki t_saki@serenegiant.com
 * Copyright (c) 2024 vschryabets@gmail.com
 *
 * File name: UVCPreview.cpp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * All files in the folder are under this Apache License, Version 2.0.
 * Files in the jni/libjpeg, jni/libusb, jin/libuvc, jni/rapidjson folder may have a different license, see the respective files.
*/

#include <stdlib.h>
#include <unistd.h>
#include "utilbase.h"
#include "UVCPreviewBase.h"
#include "libuvc/libuvc_internal.h"
#include "uvchacks.h"

#define LOCAL_DEBUG 1

UVCCaptureBase::UVCCaptureBase(uvc_device_handle_t *devh,
                               uint16_t deviceId,
                               UvcCaptureListener* previewListener,
                               int16_t framePoolSize,
                               int16_t maxFramesQueue)
        : mDeviceHandle(devh),
          requestWidth(DEFAULT_PREVIEW_WIDTH),
          requestHeight(DEFAULT_PREVIEW_HEIGHT),
          requestFps(0),
          frameWidth(DEFAULT_PREVIEW_WIDTH),
          frameHeight(DEFAULT_PREVIEW_HEIGHT),
          mIsRunning(false),
          mDeviceId(deviceId),
          mPreviewListener(previewListener),
          mFramePoolSize(framePoolSize),
          mMaxFramesQueue(maxFramesQueue) {
    pthread_mutex_init(&pool_mutex, nullptr);
    pthread_mutex_init(&preview_mutex, nullptr);
    pthread_cond_init(&preview_sync, nullptr);
}

UVCCaptureBase::~UVCCaptureBase() {
    stopCapture();
    clear_pool();
    pthread_mutex_lock(&preview_mutex);
    pthread_mutex_destroy(&preview_mutex);
    pthread_cond_destroy(&preview_sync);
    pthread_mutex_lock(&pool_mutex);
    pthread_mutex_destroy(&pool_mutex);
}

/**
 * get uvc_frame_t from frame pool
 * if pool is empty, create new frame
 * this function does not confirm the frame size
 * and you may need to confirm the size
 */
uvc_frame_t *UVCCaptureBase::get_frame(size_t data_bytes) {
    uvc_frame_t *frame = NULL;
    pthread_mutex_lock(&pool_mutex);
    {
        if (!mFramePool.empty()) {
            frame = mFramePool.back();
            mFramePool.pop_back();
        }
    }
    pthread_mutex_unlock(&pool_mutex);
    if (UNLIKELY(!frame)) {
        mAllocatedFramesCounter++;
        frame = uvc_allocate_frame(data_bytes);
    }
    return frame;
}

void UVCCaptureBase::recycle_frame(uvc_frame_t *frame) {
    if (frame == nullptr) {
        return;
    }
    pthread_mutex_lock(&pool_mutex);
    if (LIKELY(mFramePool.size() < mFramePoolSize)) {
        mFramePool.push_back(frame);
        frame = nullptr;
    }
    pthread_mutex_unlock(&pool_mutex);
    if (UNLIKELY(frame)) {
        uvc_free_frame(frame);
    }
}

void UVCCaptureBase::clear_pool() {
    pthread_mutex_lock(&pool_mutex);
    {
        for (const auto &frame: mFramePool)
            uvc_free_frame(frame);
        mFramePool.clear();
    }
    pthread_mutex_unlock(&pool_mutex);
}

inline const bool UVCCaptureBase::isRunning() const { return mIsRunning; }

int UVCCaptureBase::setPreviewSize(int width,
                                   int height,
                                   int fps,
                                   float bandwidth) {
    int result = 0;
    if ((requestWidth != width) || (requestHeight != height)) {
        requestWidth = width;
        requestHeight = height;
        requestFps = fps;

        uvc_stream_ctrl_t ctrl;
        result = uvc_get_stream_ctrl_format_size(mDeviceHandle,
                                                     &ctrl,
                                                     UVC_FRAME_FORMAT_ANY,
                                                     requestWidth,
                                                     requestHeight,
                                                     requestFps);
    }
    LOGD("setPreviewSize %dx%d bandwidth=%f fps=%d res=%d", width, height,
         bandwidth,
         requestFps,
         result);
    return result;
}

int UVCCaptureBase::startCapture() {
    int result = EXIT_FAILURE;
    if (!isRunning()) {
        mIsRunning = true;
        pthread_mutex_lock(&preview_mutex);
        {
            mPreviewThread = std::thread(&UVCCaptureBase::previewThreadFunc, this);
            result = EXIT_SUCCESS;
        }

        pthread_mutex_unlock(&preview_mutex);
        if (UNLIKELY(result != EXIT_SUCCESS)) {
            LOGW("UVCCamera::window does not exist/already running/could not create thread etc.");
            mIsRunning = false;
            pthread_mutex_lock(&preview_mutex);
            {
                pthread_cond_signal(&preview_sync);
            }
            pthread_mutex_unlock(&preview_mutex);
        }
    }
    return result;
}

int UVCCaptureBase::stopCapture() {
    bool b = isRunning();
    if (LIKELY(b)) {
        mIsRunning = false;
        pthread_cond_signal(&preview_sync);
        if (mPreviewThread.joinable()) {
            mPreviewThread.join();
        }
    }
    clearPreviewFramesQueue();
    return 0;
}

void UVCCaptureBase::uvc_preview_frame_callback(uvc_frame_t *frame, void *vptr_args) {
    std::chrono::steady_clock::time_point timestamp = std::chrono::steady_clock::now();
    auto *pCaptureBase = reinterpret_cast<UVCCaptureBase *>(vptr_args);
    if UNLIKELY(!pCaptureBase->isRunning() || !frame || !frame->frame_format || !frame->data || !frame->data_bytes) return;

    auto format = frame->frame_format;
    uint32_t expectedSize = 0;
    if (format == UVC_FRAME_FORMAT_UYVY || format == UVC_FRAME_FORMAT_YUYV) {
        expectedSize = frame->width * frame->height * 2;
    } else if (format == UVC_FRAME_FORMAT_BGR || format == UVC_FRAME_FORMAT_RGB) {
        expectedSize = frame->width * frame->height * 3;
    }

    if (UNLIKELY(
            (frame->width != pCaptureBase->frameWidth) ||
            (frame->height != pCaptureBase->frameHeight) ||
            (expectedSize != 0 && frame->data_bytes != expectedSize))) {
#if LOCAL_DEBUG
        LOGE("broken frame!:format=%d,actual_bytes=%zu(%d,%d/%d,%d), expected=%d",
             frame->frame_format, frame->data_bytes,
             frame->width, frame->height, pCaptureBase->frameWidth, pCaptureBase->frameHeight,
             expectedSize);
#endif
        pCaptureBase->onBrokenFrame(timestamp);
        return;
    }
    if (LIKELY(pCaptureBase->isRunning())) {
        uvc_frame_t *copy = pCaptureBase->get_frame(frame->data_bytes);
        if (UNLIKELY(!copy)) {
#if LOCAL_DEBUG
            LOGE("uvc_callback:unable to allocate duplicate frame!");
#endif
            return;
        }
        uvc_error_t ret = uvc_duplicate_frame(frame, copy);
        if (UNLIKELY(ret)) {
            pCaptureBase->recycle_frame(copy);
            return;
        }
        pCaptureBase->addPreviewFrame(copy, timestamp);
    }
}

void UVCCaptureBase::addPreviewFrame(uvc_frame_t *frame, std::chrono::steady_clock::time_point timestamp) {

    if (isRunning()) {
        pthread_mutex_lock(&preview_mutex);
        if (mPreviewFrames.size() < mMaxFramesQueue) {
            mPreviewFrames.push_back(
                    {
                            .mFrame = frame,
                            .mTimestamp = timestamp
                    }
            );
            frame = nullptr;
            pthread_cond_signal(&preview_sync);
        } else if (mPreviewListener != nullptr) {
            mPreviewListener->onFrameLost(mDeviceId, timestamp, UvcCaptureListener::LOST_FRAME_DROPPED);
        }
        pthread_mutex_unlock(&preview_mutex);
    }
    if (frame) {
        recycle_frame(frame);
    }
}

const UvcPreviewFrame UVCCaptureBase::waitPreviewFrame() {
    UvcPreviewFrame frame = {
            .mFrame = nullptr
    };
    pthread_mutex_lock(&preview_mutex);
    if (mPreviewFrames.empty()) {
        pthread_cond_wait(&preview_sync, &preview_mutex);
    }
    if (LIKELY(isRunning() && !mPreviewFrames.empty())) {
        frame = mPreviewFrames.front();
        mPreviewFrames.pop_front();
    }
    pthread_mutex_unlock(&preview_mutex);
    return frame;
}

void UVCCaptureBase::clearPreviewFramesQueue() {
    pthread_mutex_lock(&preview_mutex);
    {
        for (const auto &frame: mPreviewFrames) {
            recycle_frame(frame.mFrame);
        }
        mPreviewFrames.clear();
    }
    pthread_mutex_unlock(&preview_mutex);
}

void UVCCaptureBase::previewThreadFunc() {
    uvc_stream_ctrl_t ctrl;

    LOGI("previewThreadFunc frameSize=%dx%d", requestWidth, requestHeight);

    try {
        uvc_error_t result = uvc_get_stream_ctrl_format_size(
                mDeviceHandle,
                &ctrl,
                UVC_FRAME_FORMAT_ANY,
                requestWidth,
                requestHeight,
                requestFps);
        if (result)
            throw UvcPreviewFailed(UvcPreviewFailed::NO_FORMAT, "Can't find format");

        result = uvc_start_streaming(
                mDeviceHandle,
                &ctrl,
                uvc_preview_frame_callback,
                (void *) this, 0);

        if (result)
            throw UvcPreviewFailed(UvcPreviewFailed::START_STREAM_FAILED, "Can't start streaming");
// TODO i've always got 640x480 here
//        const uvc_format_desc_t *format_desc = uvc_get_format_descs(mDeviceHandle);
//
//        if (LIKELY(format_desc)) {
//            const uvc_frame_desc_t *frame_desc = format_desc->frame_descs;
//            frameWidth = frame_desc->wWidth;
//            frameHeight = frame_desc->wHeight;
//            LOGI("ASD prepare_preview got %dx%d@%s", frameWidth, frameHeight,
//                 (!requestMode ? "YUYV" : "MJPEG"));
//        } else {
//            LOGE("Can't get current format");
            frameWidth = requestWidth;
            frameHeight = requestHeight;
//        }

        clearPreviewFramesQueue();

        if (mPreviewListener != nullptr)
            mPreviewListener->onPrepared(mDeviceId, frameWidth, frameHeight);

        while (LIKELY(isRunning())) {
            auto frame = waitPreviewFrame();
            if (isRunning() &&
                mPreviewListener != nullptr &&
                frame.mFrame != nullptr)
                mPreviewListener->handleFrame(mDeviceId, frame);
            recycle_frame(frame.mFrame);
        }
        if (mPreviewListener != nullptr) {
            mPreviewListener->onFinished(mDeviceId);
        }
        uvc_stop_streaming(mDeviceHandle);
    } catch (UvcPreviewFailed err) {
        if (mPreviewListener != nullptr) {
            mPreviewListener->onFailed(mDeviceId, err);
        }
        LOGE("Exception %s", err.what());
    }

}

void UVCCaptureBase::onBrokenFrame(std::chrono::steady_clock::time_point point) {
    mBrokenFramesCounter++;
    if (mPreviewListener != nullptr) {
        mPreviewListener->onFrameLost(mDeviceId, point, UvcCaptureListener::LOST_FRAME_BROKEN);
    }
}

UvcPreviewFailed::UvcPreviewFailed(UvcPreviewFailed::Type error, std::string decsription):
        error(error), description(decsription + " code " + std::to_string(error)) {
}

const char *UvcPreviewFailed::what() noexcept {
    return description.c_str();
}

UvcPreviewFailed::UvcPreviewFailed(UvcPreviewFailed &src) {
    this->error = src.error;
    this->description = src.description;
}
