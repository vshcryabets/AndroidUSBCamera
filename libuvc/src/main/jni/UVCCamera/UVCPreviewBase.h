/*
 * UVCCamera
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014-2017 saki t_saki@serenegiant.com
 * Copyright (c) 2024 vshcryabets@gmail.com
 *
 * File name: UVCPreview.h
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

#include <jni.h>
#include "libusb.h"
#include "libuvc.h"
#include "utilbase.h"
#include <pthread.h>
#include <stdint.h>
#include <mutex>
#include <condition_variable>
#include <list>

#define DEFAULT_PREVIEW_WIDTH 640
#define DEFAULT_PREVIEW_HEIGHT 480
#define DEFAULT_PREVIEW_FPS_MIN 1
#define DEFAULT_PREVIEW_FPS_MAX 30
#define DEFAULT_PREVIEW_MODE 0
#define DEFAULT_BANDWIDTH 1.0f
#define MAX_FRAME 4
#define PREVIEW_PIXEL_BYTES 4    // RGBA/RGBX
#define FRAME_POOL_SZ MAX_FRAME + 2


typedef uvc_error_t (*convFunc_t)(uvc_frame_t *in, uvc_frame_t *out);

#define PIXEL_FORMAT_RAW 0		// same as PIXEL_FORMAT_YUV
#define PIXEL_FORMAT_YUV 1
#define PIXEL_FORMAT_RGB565 2
#define PIXEL_FORMAT_RGBX 3
#define PIXEL_FORMAT_YUV20SP 4
#define PIXEL_FORMAT_NV21 5		// YVU420SemiPlanar

class UVCPreviewBase {
protected:
	uvc_device_handle_t *mDeviceHandle;
	volatile bool mIsRunning;
	int requestWidth, requestHeight, requestMode;
	int requestMinFps, requestMaxFps;
	float requestBandwidth;
	uint16_t frameWidth, frameHeight;
	int frameMode;
	size_t frameBytes;
	pthread_t preview_thread;
	pthread_mutex_t preview_mutex;
	pthread_cond_t preview_sync;
	std::list<uvc_frame_t *> previewFrames;
//	size_t previewBytes;
//	convFunc_t mFrameCallbackFunc;
//	int mPixelFormat;
//	size_t callbackPixelBytes;
	pthread_mutex_t pool_mutex;
    std::list<uvc_frame_t *> mFramePool;
    volatile uint16_t allocatedFramesCounter = 0;
private:
	void clear_pool();
	static void uvc_preview_frame_callback(uvc_frame_t *frame, void *vptr_args);
	void addPreviewFrame(uvc_frame_t *frame);
	void clearPreviewFramesQueue();
	static void *preview_thread_func(void *vptr_args);
	int prepare_preview(uvc_stream_ctrl_t *ctrl);
	void do_preview(uvc_stream_ctrl_t *ctrl);
protected:
    uvc_frame_t *get_frame(size_t data_bytes);
    void recycle_frame(uvc_frame_t *frame);
    uvc_frame_t *waitPreviewFrame();

    // will be called on each frame from UVC
    virtual void handleFrame(uvc_frame_t *frame) = 0;

    // will be called once from worker thread of the UVCPreviewBase
    virtual void onPreviewPrepared(uint16_t frameWidth, uint16_t  frameHeight) = 0;
    
public:
	UVCPreviewBase(uvc_device_handle_t *devh);
	virtual ~UVCPreviewBase();

	inline const bool isRunning() const;
	int setPreviewSize(int width, int height, int min_fps, int max_fps, int mode, float bandwidth = 1.0f);
	int startPreview();
	virtual int stopPreview();
};
