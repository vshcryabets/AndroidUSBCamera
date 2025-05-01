/*
 * UVCCamera
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014-2017 saki t_saki@serenegiant.com
 * Copyright (c) 2024 vshcryabets@gmail.com
 *
 * File name: UVCCamera.cpp
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

#define LOG_TAG "UVCCamera"
#if 1    // デバッグ情報を出さない時1
#ifndef LOG_NDEBUG
#define    LOG_NDEBUG        // LOGV/LOGD/MARKを出力しない時
#endif
#undef USE_LOGALL            // 指定したLOGxだけを出力
#else
#define USE_LOGALL
#undef LOG_NDEBUG
#undef NDEBUG
#define GET_RAW_DESCRIPTOR
#endif

#include <stdlib.h>
#include <unistd.h>
#include "UVCCamera.h"
#include "libuvc/libuvc_internal.h"
#include "uvchacks.h"
#include <vector>

#define    LOCAL_DEBUG 0

UVCCamera::UVCCamera()
        : mFd(0),
          mUsbFs(""),
          mContext(NULL),
          mDevice(NULL),
          mDeviceHandle(NULL),
          mPreview(nullptr) {
    clearCameraParams();
}

UVCCamera::~UVCCamera() {
    release();
    if (mContext) {
        uvc_exit(mContext);
        mContext = NULL;
    }
}

void UVCCamera::clearCameraParams() {
    if (LIKELY(mCameraConfig)) {
        mCameraConfig->clearCameraParams();
    }
}

//======================================================================
/**
 * カメラへ接続する
 */
int UVCCamera::connect(int vid, int pid, int fd, int busnum, int devaddr, std::string usbfs) {
    uvc_error_t result = UVC_ERROR_BUSY;
    if (!mDeviceHandle && fd) {
        mUsbFs = usbfs;
        if (UNLIKELY(!mContext)) {
            struct libusb_context *libusb_context;
            int res = libusb_init2(&libusb_context, usbfs.c_str());
            if (res != LIBUSB_SUCCESS) {
                LOGE("failed to init USB context");
                RETURN(result, int);
            }

            result = uvc_init(&mContext, libusb_context);
            if (UNLIKELY(result < 0)) {
                LOGE("failed to init libuvc");
                RETURN(result, int);
            }
        }
        // カメラ機能フラグをクリア
        clearCameraParams();
        fd = dup(fd);
        // 指定したvid,idを持つデバイスを検索, 見つかれば0を返してmDeviceに見つかったデバイスをセットする(既に1回uvc_ref_deviceを呼んである)
//		result = uvc_find_device(mContext, &mDevice, vid, pid, NULL, fd);
        result = uvchack_get_device_with_fd(mContext, &mDevice, vid, pid, NULL, fd, busnum, devaddr);
        if (LIKELY(!result)) {
            // カメラのopen処理
            result = uvc_open(mDevice, &mDeviceHandle);
            if (LIKELY(!result)) {
                mCameraConfig = std::shared_ptr<UVCCameraAdjustments>(new UVCCameraAdjustments(mDeviceHandle));
                mFd = fd;
                mPreview = constructPreview(mDeviceHandle);
            } else {
                LOGE("could not open camera:err=%d", result);
                uvc_unref_device(mDevice);
                mDevice = NULL;
                mDeviceHandle = NULL;
                close(fd);
            }
        } else {
            LOGE("could not find camera:err=%d", result);
            close(fd);
        }
    } else {
        // カメラが既にopenしている時
        LOGW("camera is already opened. you should release first");
    }
    RETURN(result, int);
}

int UVCCamera::release() {
    if (LIKELY(mPreview)) {
        mPreview->stopCapture();
    }
    if (LIKELY(mDeviceHandle)) {
        uvc_close(mDeviceHandle);
        mDeviceHandle = NULL;
    }
    if (LIKELY(mDevice)) {
        MARK("カメラを開放");
        uvc_unref_device(mDevice);
        mDevice = NULL;
    }
    // カメラ機能フラグをクリア
    clearCameraParams();
    mUsbFs = "";
    if (mFd != 0) {
        close(mFd);
        mFd = 0;
    }
    return 0;
}

std::vector<UvcCameraResolution> UVCCamera::getSupportedSize() {
    auto result = std::vector<UvcCameraResolution>();
    if (!mDeviceHandle)
        return result;
    if (mDeviceHandle->info->stream_ifs) {
        int stream_idx = 0;
        for (auto *stream_if = mDeviceHandle->info->stream_ifs;
             stream_if;
             stream_if = stream_if->next) {
            ++stream_idx;
            for (auto *fmt_desc = stream_if->format_descs;
                 fmt_desc;
                 fmt_desc = fmt_desc->next) {
                switch (fmt_desc->bDescriptorSubtype) {
                    case UVC_VS_FORMAT_UNCOMPRESSED:
                    case UVC_VS_FORMAT_MJPEG:
                        for (const auto *frame_desc = fmt_desc->frame_descs;
                             frame_desc;
                             frame_desc = frame_desc->next) {
                            std::vector<uint32_t> intervals;
                            for (auto interval = frame_desc->intervals; *interval; ++interval) {
                                intervals.push_back(*interval);
                            }
                            result.push_back({
                                                     .id =  fmt_desc->bFormatIndex,
                                                     .subtype = (uint8_t) fmt_desc->bDescriptorSubtype,
                                                     .frameIndex = fmt_desc->bDefaultFrameIndex,
                                                     .width = frame_desc->wWidth,
                                                     .height = frame_desc->wHeight,
                                                     .frameIntervals = intervals
                                             });
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }
    return result;
}

//int UVCCamera::setFrameCallback(JNIEnv *env, jobject frame_callback_obj, int pixel_format) {
//    int result = EXIT_FAILURE;
//    if (mPreview) {
//        result = mPreview->setFrameCallback(env, frame_callback_obj, pixel_format);
//    }
//    return result;
//}

int UVCCamera::getCtrlSupports(uint64_t *supports) {
    uvc_error_t ret = UVC_ERROR_NOT_FOUND;
    if (LIKELY(mDeviceHandle)) {
        if (!mCameraConfig->mCtrlSupports) {
            // 何個あるのかわからへんねんけど、試した感じは１個みたいやからとりあえず先頭のを返す
            const uvc_input_terminal_t *input_terminals = uvc_get_input_terminals(mDeviceHandle);
            const uvc_input_terminal_t *it;
            DL_FOREACH(input_terminals, it) {
                if (it) {
                    mCameraConfig->mCtrlSupports = it->bmControls;
                    MARK("getCtrlSupports=%lx", (unsigned long) mCtrlSupports);
                    ret = UVC_SUCCESS;
                    break;
                }
            }
        } else
            ret = UVC_SUCCESS;
    }
    if (supports)
        *supports = mCameraConfig->mCtrlSupports;
    RETURN(ret, int);
}

int UVCCamera::getProcSupports(uint64_t *supports) {
    uvc_error_t ret = UVC_ERROR_NOT_FOUND;
    if (LIKELY(mDeviceHandle)) {
        if (!mCameraConfig->mPUSupports) {
            // 何個あるのかわからへんねんけど、試した感じは１個みたいやからとりあえず先頭のを返す
            const uvc_processing_unit_t *proc_units = uvc_get_processing_units(mDeviceHandle);
            const uvc_processing_unit_t *pu;
            DL_FOREACH(proc_units, pu) {
                if (pu) {
                    mCameraConfig->mPUSupports = pu->bmControls;
                    MARK("getProcSupports=%lx", (unsigned long) mPUSupports);
                    ret = UVC_SUCCESS;
                    break;
                }
            }
        } else
            ret = UVC_SUCCESS;
    }
    if (supports)
        *supports = mCameraConfig->mPUSupports;
    RETURN(ret, int);
}

std::shared_ptr<UVCCaptureBase> UVCCamera::getPreview() const {
    return mPreview;
}

std::shared_ptr<UVCCameraAdjustments> UVCCamera::getAdjustments() const {
    return mCameraConfig;
}

uvc_device_t *UVCCamera::getUvcDevice() {
    return mDevice;
}

uvc_device_handle_t *UVCCamera::getUvcDeviceHandle() {
    return mDeviceHandle;
}

UVCCameraJniImpl::UVCCameraJniImpl() : UVCCamera() {

}

std::shared_ptr<UVCCaptureBase> UVCCameraJniImpl::constructPreview(uvc_device_handle_t *deviceHandle) {
    return std::shared_ptr<UVCCaptureBase>(new UVCPreviewJni(deviceHandle));
}
