/*
 * UVCCamera
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014-2017 saki t_saki@serenegiant.com
 *
 * File name: utilbase.h
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

#ifndef UTILBASE_H_
#define UTILBASE_H_

#include <stdio.h>

#define		RETURN(code,type)	{type RESULT = code; return RESULT;}

#if defined(__GNUC__)
// the macro for branch prediction optimaization for gcc(-O2/-O3 required)
#define		CONDITION(cond)				((__builtin_expect((cond)!=0, 0)))
#define		LIKELY(x)					((__builtin_expect(!!(x), 1)))	// x is likely true
#define		UNLIKELY(x)					((__builtin_expect(!!(x), 0)))	// x is likely false
#else
#define		CONDITION(cond)				((cond))
#define		LIKELY(x)					((x))
#define		UNLIKELY(x)					((x))
#endif

#define LOGV(FMT, ...) printf(FMT, ## __VA_ARGS__);printf("\n")
#define LOGD(FMT, ...) printf(FMT, ## __VA_ARGS__);printf("\n")
#define LOGI(FMT, ...) printf(FMT, ## __VA_ARGS__);printf("\n")
#define LOGW(FMT, ...) printf(FMT, ## __VA_ARGS__);printf("\n")
#define LOGF(FMT, ...) printf(FMT, ## __VA_ARGS__);printf("\n")
#define LOGE(FMT, ...) printf(FMT, ## __VA_ARGS__);printf("\n")
#define MARK(...)

#ifdef __ANDROID__
#include "utilbase_android.h"
#endif

#define		RET(code)			{LOGD("end"); return code;}
#define		EXIT()				{LOGD("end"); return;}
#define		PRE_EXIT()			LOGD("end")

#endif /* UTILBASE_H_ */
