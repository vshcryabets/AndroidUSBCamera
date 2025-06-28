/*
 * Copyright 2025 vschryabets@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "LoadJpegImageFromAssetsUseCase.h"
#include <android/asset_manager.h>

LoadJpegImageUseCase::Result LoadJpegImageFromAssetsUseCase::load(std::string imageId) {
    auto asset = AAssetManager_open(assetManagerObj, imageId.c_str(), AASSET_MODE_BUFFER);
    if (asset == nullptr) {
        return { .buffer = nullptr, .size = 0};
    } else {
        auto *srcBuffer = AAsset_getBuffer(asset);
        size_t size = AAsset_getLength(asset);
        auto* buffer = new uint8_t[size];
        memcpy(buffer, srcBuffer, size);
        AAsset_close(asset);
        return { .buffer = buffer, .size = size};
    }

}
