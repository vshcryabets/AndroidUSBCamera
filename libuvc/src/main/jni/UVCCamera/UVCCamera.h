/*
 * UVCCamera
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014-2017 saki t_saki@serenegiant.com
 *
 * File name: UVCCamera.h
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

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <android/native_window.h>
#include "UVCCameraAdjustments.h"
#include "UVCPreviewJni.h"
#include <memory>
#include <chrono>
#include <vector>

struct UvcCameraResolution {
    const uint8_t id;
    const uint8_t subtype;
    const uint8_t frameIndex;
    const uint16_t width;
    const uint16_t height;
};

class UVCCamera {
private:
	std::string mUsbFs;
	uvc_context_t *mContext;
	int mFd;
	uvc_device_t *mDevice;
	uvc_device_handle_t *mDeviceHandle;
    std::shared_ptr<UVCPreviewBase> mPreview;
    std::shared_ptr<UVCCameraAdjustments> mCameraConfig;
private:
	void clearCameraParams();
protected:
    virtual std::shared_ptr<UVCPreviewBase> constructPreview(uvc_device_handle_t *deviceHandle) = 0;
public:
	UVCCamera();
	virtual ~UVCCamera();

	int connect(int vid, int pid, int fd, int busnum, int devaddr, std::string usbfs);
	int release();

	std::vector<UvcCameraResolution> getSupportedSize();
	int getCtrlSupports(uint64_t *supports);
	int getProcSupports(uint64_t *supports);
    std::shared_ptr<UVCPreviewBase> getPreview();
    std::shared_ptr<UVCCameraAdjustments> getAdjustments();
};

class UVCCameraJniImpl : public UVCCamera {
protected:
    virtual std::shared_ptr<UVCPreviewBase> constructPreview(uvc_device_handle_t *deviceHandle);
public:
    UVCCameraJniImpl();
    virtual ~UVCCameraJniImpl(){};
};