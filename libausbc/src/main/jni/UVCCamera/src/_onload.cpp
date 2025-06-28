/*
 * UVCCamera
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014-2017 saki t_saki@serenegiant.com
 *               2025 vschryabets@gmail.com
 * File name: _onload.cpp
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

#include "_onload.h"
#include "utilbase.h"
#include "LoadJpegImageFromAssetsUseCase.h"

#ifndef ABI
    #define ABI 0
#endif
#define LOCAL_DEBUG 1


DI* DI::instance = nullptr;

class AndroidDi: public DI {
private:
    jobject globalObjectAssetManager {nullptr};
    AAssetManager* nativeAssetManager {nullptr};
    LoadJpegImageFromAssetsUseCase *imageLoader {nullptr};
#if defined(USE_LIBJPEG)
    DecodeJpegImageLibJpeg9UseCase decoder;
#elif defined(USE_TURBOJPEG)
    DecodeJpegImageTurboJpegUseCase decoder;
#endif
    SaveBitmapImageToFileUseCase imageSaver;
    UseCases *useCases;
public:
    AndroidDi(JNIEnv *env, jobject assetManager) {
        globalObjectAssetManager = env->NewGlobalRef(assetManager);
        nativeAssetManager = AAssetManager_fromJava(env, globalObjectAssetManager);
        imageLoader = new LoadJpegImageFromAssetsUseCase(nativeAssetManager);
        useCases = new UseCases {
                .imageLoader = imageLoader,
                .imageDecoder = &decoder,
                .imageSaver = &imageSaver
        };
    }

    UseCases* getUseCases() override {
        return useCases;
    }
};



extern int register_uvccamera(JNIEnv *env);

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
#if LOCAL_DEBUG
    LOGD("JNI_OnLoad");
#endif
    LOGI("UVCCamera library abi=%d", ABI);
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    // register native methods
    int result = register_uvccamera(env);
	setVM(vm);
#if LOCAL_DEBUG
    LOGD("JNI_OnLoad:finshed:result=%d", result);
#endif
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiangdg_demo_DemoApplication_initDI(JNIEnv *env, jobject thiz, jobject assetManager) {
    DI::setInstance(new AndroidDi(env, assetManager));
}