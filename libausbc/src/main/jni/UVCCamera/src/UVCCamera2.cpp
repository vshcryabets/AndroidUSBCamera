/*
 * UVCCamera
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2025 vshcryabets@gmail.com
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
 * Files in the jni/libjpeg, jni/libusb, jin/libuvc folder may have a different license, see the respective files.
*/
#include "UVCCamera2.h"

UVCCamera2::UVCCamera2()
{

}

UVCCamera2::~UVCCamera2()
{

}

std::vector<auvc::FrameFormat> UVCCamera2::getSupportedFrameFormats() const
{
    return {
        auvc::FrameFormat::RGBX
    };
}

std::future<void> UVCCamera2::stopProducing()
{
    return auvc::completed();
}

std::future<void> UVCCamera2::close()
{
    return auvc::completed();
}

std::map<uint16_t, std::vector<Source::Resolution>> UVCCamera2::getSupportedResolutions() const
{
    return {};
}