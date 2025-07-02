/*
 * UVCCamera
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014-2017 saki t_saki@serenegiant.com
 * Copyright (c) 2024 vshcryabets@gmail.com
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

#include "Camera.h"
#include "Source.h"

class UVCCamera2: public Camera, public PushSource {
public:
    struct ConnectConfiguration {
        int vid;
        int pid;
        int fd;
        int busnum;
        int devaddr;
        std::string usbfs;
    };
public:
    UVCCamera2();
	virtual ~UVCCamera2();
    std::map<uint16_t, std::vector<Resolution>> getSupportedResolutions();
    std::vector<FrameFormat> getSupportedFrameFormats() override;
    void stopCapturing() override;
    void close() override;
};
