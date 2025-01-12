/*
 * UVCPreviewJni
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014-2017 saki t_saki@serenegiant.com
 * Copyright (c) 2024 vschryabets@gmail.com
 *
 * File name: UVCPreviewJni.cpp
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
 * Files in the jni/libjpeg, jni/libusb, jin/libuvc folder may have a different license, see the respective files.
*/

#include <stdlib.h>
#include "UVCPreviewJni.h"

int UVCPreviewJni::setPreviewDisplay(ANativeWindow *preview_window) {
    pthread_mutex_lock(&preview_mutex);
    {
        if (mPreviewWindow != preview_window) {
            if (mPreviewWindow)
                ANativeWindow_release(mPreviewWindow);
            mPreviewWindow = preview_window;
            if (LIKELY(mPreviewWindow)) {
                ANativeWindow_setBuffersGeometry(mPreviewWindow,
                                                 frameWidth, frameHeight, WINDOW_FORMAT_RGBA_8888);
            }
        }
    }
    pthread_mutex_unlock(&preview_mutex);
    RETURN(0, int);
}

int UVCPreviewJni::setCaptureDisplay(ANativeWindow *capture_window) {
    /*pthread_mutex_lock(&capture_mutex);
    {
        if (isRunning() && isCapturing()) {
            mIsCapturing = false;
            if (mCaptureWindow) {
                pthread_cond_signal(&capture_sync);
                pthread_cond_wait(&capture_sync, &capture_mutex);    // wait finishing capturing
            }
        }
        if (mCaptureWindow != capture_window) {
            // release current Surface if already assigned.
            if (UNLIKELY(mCaptureWindow))
                ANativeWindow_release(mCaptureWindow);
            mCaptureWindow = capture_window;
            // if you use Surface came from MediaCodec#createInputSurface
            // you could not change window format at least when you use
            // ANativeWindow_lock / ANativeWindow_unlockAndPost
            // to write frame data to the Surface...
            // So we need check here.
            if (mCaptureWindow) {
                int32_t window_format = ANativeWindow_getFormat(mCaptureWindow);
                if ((window_format != WINDOW_FORMAT_RGB_565)
                    && (previewFormat == WINDOW_FORMAT_RGB_565)) {
                    LOGE("window format mismatch, cancelled movie capturing.");
                    ANativeWindow_release(mCaptureWindow);
                    mCaptureWindow = NULL;
                }
            }
        }
    }
    pthread_mutex_unlock(&capture_mutex);*/
    return 0;
}

UVCPreviewJni::UVCPreviewJni(uvc_device_handle_t *devh)
        : UVCCaptureBase(devh, 1, this, 8, 4),
          mPreviewWindow(NULL),
          mCaptureWindow(NULL),
          mFrameCallbackObj(NULL) {

}

UVCPreviewJni::~UVCPreviewJni() {
    if (mPreviewWindow)
        ANativeWindow_release(mPreviewWindow);
    mPreviewWindow = NULL;
    if (mCaptureWindow)
        ANativeWindow_release(mCaptureWindow);
    mCaptureWindow = NULL;
}

int UVCPreviewJni::stopCapture() {
    auto res = UVCCaptureBase::stopCapture();
    clearDisplay();
    // check preview mutex available
    if (pthread_mutex_lock(&preview_mutex) == 0) {
        if (mPreviewWindow) {
            ANativeWindow_release(mPreviewWindow);
            mPreviewWindow = NULL;
        }
        pthread_mutex_unlock(&preview_mutex);
    }
//    if (pthread_mutex_lock(&capture_mutex) == 0) {
//        if (mCaptureWindow) {
//            ANativeWindow_release(mCaptureWindow);
//            mCaptureWindow = NULL;
//        }
//        pthread_mutex_unlock(&capture_mutex);
//    }
    return res;
}

int UVCPreviewJni::setFrameCallback(JNIEnv *env, jobject frame_callback_obj, int pixel_format) {
//    pthread_mutex_lock(&capture_mutex);
//    {
//        if (isRunning() && isCapturing()) {
//            mIsCapturing = false;
//            if (mFrameCallbackObj) {
//                pthread_cond_signal(&capture_sync);
//                pthread_cond_wait(&capture_sync, &capture_mutex);    // wait finishing capturing
//            }
//        }
//        if (!env->IsSameObject(mFrameCallbackObj, frame_callback_obj)) {
//            iframecallback_fields.onFrame = NULL;
//            if (mFrameCallbackObj) {
//                env->DeleteGlobalRef(mFrameCallbackObj);
//            }
//            mFrameCallbackObj = frame_callback_obj;
//            if (frame_callback_obj) {
//                // get method IDs of Java object for callback
//                jclass clazz = env->GetObjectClass(frame_callback_obj);
//                if (LIKELY(clazz)) {
//                    iframecallback_fields.onFrame = env->GetMethodID(clazz,
//                                                                     "onFrame", "(Ljava/nio/ByteBuffer;)V");
//                } else {
//                    LOGW("failed to get object class");
//                }
//                env->ExceptionClear();
//                if (!iframecallback_fields.onFrame) {
//                    LOGE("Can't find IFrameCallback#onFrame");
//                    env->DeleteGlobalRef(frame_callback_obj);
//                    mFrameCallbackObj = frame_callback_obj = NULL;
//                }
//            }
//        }
//        if (frame_callback_obj) {
//            mPixelFormat = pixel_format;
//            callbackPixelFormatChanged();
//        }
//    }
//    pthread_mutex_unlock(&capture_mutex);
    return 0;
}

void UVCPreviewJni::clearDisplay() {
    ANativeWindow_Buffer buffer;
//    pthread_mutex_lock(&capture_mutex);
//    {
//        if (LIKELY(mCaptureWindow)) {
//            if (LIKELY(ANativeWindow_lock(mCaptureWindow, &buffer, NULL) == 0)) {
//                uint8_t *dest = (uint8_t *) buffer.bits;
//                const size_t bytes = buffer.width * PREVIEW_PIXEL_BYTES;
//                const int stride = buffer.stride * PREVIEW_PIXEL_BYTES;
//                for (int i = 0; i < buffer.height; i++) {
//                    memset(dest, 0, bytes);
//                    dest += stride;
//                }
//                ANativeWindow_unlockAndPost(mCaptureWindow);
//            }
//        }
//    }
//    pthread_mutex_unlock(&capture_mutex);
    pthread_mutex_lock(&preview_mutex);
    {
        if (LIKELY(mPreviewWindow)) {
            if (LIKELY(ANativeWindow_lock(mPreviewWindow, &buffer, NULL) == 0)) {
                uint8_t *dest = (uint8_t *) buffer.bits;
                const size_t bytes = buffer.width * PREVIEW_PIXEL_BYTES;
                const int stride = buffer.stride * PREVIEW_PIXEL_BYTES;
                for (int i = 0; i < buffer.height; i++) {
                    memset(dest, 0, bytes);
                    dest += stride;
                }
                ANativeWindow_unlockAndPost(mPreviewWindow);
            }
        }
    }
    pthread_mutex_unlock(&preview_mutex);
}

void UVCPreviewJni::handleFrame(uint16_t deviceId,
                                const UvcPreviewFrame &frame) {
    uvc_error_t result;
    if (LIKELY(frame.mFrame)) {
        auto rgbFrame = uvc_allocate_frame(frame.mFrame->width * frame.mFrame->height * 3);
        result = uvc_any2rgb(frame.mFrame, rgbFrame);
        if (LIKELY(!result)) {
            draw_preview_rgb(rgbFrame);
        }
        uvc_free_frame(rgbFrame);
    }
}

void UVCPreviewJni::onPrepared(uint16_t deviceId,
                                      uint16_t frameWidth,
                                      uint16_t frameHeight) {
    LOGI("onPreviewPrepared %d %dx%d", deviceId, frameWidth, frameHeight);
    pthread_mutex_lock(&preview_mutex);
    if (LIKELY(mPreviewWindow)) {
        ANativeWindow_setBuffersGeometry(mPreviewWindow,
                                         frameWidth, frameHeight, WINDOW_FORMAT_RGBA_8888);
    }
    pthread_mutex_unlock(&preview_mutex);

}

// changed to return original frame instead of returning converted frame even if convert_func is not null.
void UVCPreviewJni::draw_preview_rgb(
        uvc_frame_t *frame) {
    ANativeWindow_Buffer buffer;
    // source = frame data
    pthread_mutex_lock(&preview_mutex);
    if (mPreviewWindow != nullptr) {
        if (LIKELY(ANativeWindow_lock(mPreviewWindow, &buffer, NULL) == 0)) {
            // use lower transfer bytes
            const int w = frame->width < buffer.width ? frame->width : buffer.width;
            // use lower height
            const int h = frame->height < buffer.height ? frame->height : buffer.height;
            // transfer from frame data to the Surface
            uint8_t *srcBuffer = (uint8_t*)frame->data;
            uint32_t *dest = (uint32_t *) buffer.bits;
            uint32_t srcOffset = 0;
            uint32_t dstOffset = 0;
            // TODO optimize next code
            for (uint16_t sy = 0; sy < h; sy++) {
                for (uint16_t sx = 0; sx < w; sx++) {
                    uint32_t rgb = srcBuffer[srcOffset] << 0 |
                            srcBuffer[srcOffset+1] << 8 |
                            srcBuffer[srcOffset+2] << 16;
                    srcBuffer += 3;
                    dest[dstOffset + sx] = 0xFF000000 | rgb;
                }
                dstOffset += buffer.stride;
            }
            ANativeWindow_unlockAndPost(mPreviewWindow);
        }
    }
    pthread_mutex_unlock(&preview_mutex);
}

void UVCPreviewJni::onFinished(uint16_t deviceId) {
    LOGD("onPreviewFinished");
}

void UVCPreviewJni::onFrameLost(uint16_t deviceId, std::chrono::steady_clock::time_point timestamp, uint8_t reason) {
    LOGD("onFrameDropped %lld reason = %d", timestamp.time_since_epoch(), reason);
}

void UVCPreviewJni::onFailed(uint16_t deviceId, UvcPreviewFailed error) {
    LOGE("onPreviewFailed %d %s", deviceId, error.what());
}
