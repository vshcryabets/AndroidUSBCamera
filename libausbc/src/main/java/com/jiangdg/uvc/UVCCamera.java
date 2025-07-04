/*
 *  UVCCamera
 *  library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014-2017 saki t_saki@serenegiant.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 *  All files in the folder are under this Apache License, Version 2.0.
 *  Files in the libjpeg-turbo, libusb, libuvc, rapidjson folder
 *  may have a different license, see the respective files.
 */
package com.jiangdg.uvc;

import android.graphics.SurfaceTexture;
import android.hardware.usb.UsbDevice;
import android.text.TextUtils;
import android.view.Surface;
import android.view.SurfaceHolder;

import com.jiangdg.usb.USBMonitor;
import com.jiangdg.usb.UsbControlBlock;
import com.vsh.uvc.UvcCameraResolution;
import com.vsh.uvc.UvcVsDeskSubtype;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import timber.log.Timber;

public class UVCCamera implements IUvcCamera {
	public static boolean DEBUG = false;	// TODO set false when releasing
	private static final String TAG = UVCCamera.class.getSimpleName();
	private static final String DEFAULT_USBFS = "/dev/bus/usb";

	public static final int DEFAULT_PREVIEW_FPS = 0;
	public static final float DEFAULT_BANDWIDTH = 1.0f;
	public static final UvcFrameFormat DEFAULT_PREVIEW_MODE = UvcFrameFormat.FRAME_FORMAT_MJPEG;

	private static boolean isLoaded;
	static {
		if (!isLoaded) {
			System.loadLibrary("native");
			isLoaded = true;
		}
	}

	private com.jiangdg.usb.UsbControlBlock mCtrlBlock;
    protected long mControlSupports;			// カメラコントロールでサポートしている機能フラグ
    protected long mProcSupports;				// プロセッシングユニットでサポートしている機能フラグ
    protected UvcFrameFormat mCurrentFrameFormat = UvcFrameFormat.FRAME_FORMAT_MJPEG;
	protected int mCurrentWidth = 640, mCurrentHeight = 480;
	protected float mCurrentBandwidthFactor = DEFAULT_BANDWIDTH;
    protected Map<UvcVsDeskSubtype, List<UvcCameraResolution>> mSupportedSize = Collections.emptyMap();
	// these fields from here are accessed from native code and do not change name and remove
    protected long mNativePtr;
    protected int mScanningModeMin, mScanningModeMax, mScanningModeDef;
    protected int mExposureModeMin, mExposureModeMax, mExposureModeDef;
    protected int mExposurePriorityMin, mExposurePriorityMax, mExposurePriorityDef;
    protected int mExposureMin, mExposureMax, mExposureDef;
    protected int mAutoFocusMin, mAutoFocusMax, mAutoFocusDef;
    protected int mFocusMin, mFocusMax, mFocusDef;
    protected int mFocusRelMin, mFocusRelMax, mFocusRelDef;
    protected int mFocusSimpleMin, mFocusSimpleMax, mFocusSimpleDef;
    protected int mIrisMin, mIrisMax, mIrisDef;
    protected int mIrisRelMin, mIrisRelMax, mIrisRelDef;
    protected int mPanMin, mPanMax, mPanDef;
    protected int mTiltMin, mTiltMax, mTiltDef;
    protected int mRollMin, mRollMax, mRollDef;
    protected int mPanRelMin, mPanRelMax, mPanRelDef;
    protected int mTiltRelMin, mTiltRelMax, mTiltRelDef;
    protected int mRollRelMin, mRollRelMax, mRollRelDef;
    protected int mPrivacyMin, mPrivacyMax, mPrivacyDef;
    protected int mAutoWhiteBlanceMin, mAutoWhiteBlanceMax, mAutoWhiteBlanceDef;
    protected int mAutoWhiteBlanceCompoMin, mAutoWhiteBlanceCompoMax, mAutoWhiteBlanceCompoDef;
    protected int mWhiteBlanceMin, mWhiteBlanceMax, mWhiteBlanceDef;
    protected int mWhiteBlanceCompoMin, mWhiteBlanceCompoMax, mWhiteBlanceCompoDef;
    protected int mWhiteBlanceRelMin, mWhiteBlanceRelMax, mWhiteBlanceRelDef;
    protected int mBacklightCompMin, mBacklightCompMax, mBacklightCompDef;
    protected int mBrightnessMin, mBrightnessMax, mBrightnessDef;
    protected int mContrastMin, mContrastMax, mContrastDef;
    protected int mSharpnessMin, mSharpnessMax, mSharpnessDef;
    protected int mGainMin, mGainMax, mGainDef;
    protected int mGammaMin, mGammaMax, mGammaDef;
    protected int mSaturationMin, mSaturationMax, mSaturationDef;
    protected int mHueMin, mHueMax, mHueDef;
    protected int mZoomMin, mZoomMax, mZoomDef;
    protected int mZoomRelMin, mZoomRelMax, mZoomRelDef;
    protected int mPowerlineFrequencyMin, mPowerlineFrequencyMax, mPowerlineFrequencyDef;
    protected int mMultiplierMin, mMultiplierMax, mMultiplierDef;
    protected int mMultiplierLimitMin, mMultiplierLimitMax, mMultiplierLimitDef;
    protected int mAnaXLogWrapperVideoStandardMin, mAnaXLogWrapperVideoStandardMax, mAnaXLogWrapperVideoStandardDef;
    protected int mAnaXLogWrapperVideoLockStateMin, mAnaXLogWrapperVideoLockStateMax, mAnaXLogWrapperVideoLockStateDef;
    // until here
    /**
     * the sonctructor of this class should be call within the thread that has a looper
     * (UI thread or a thread that called Looper.prepare)
     */
    public UVCCamera() {
    	mNativePtr = nativeCreate();
    	mSupportedSize = null;
	}

    /**
     * connect to a UVC camera
     * USB permission is necessary before this method is called
     */
	@Override
    public synchronized void open(final UsbControlBlock ctrlBlock) {
    	int result = -2;
		close();
    	try {
			mCtrlBlock = ctrlBlock.clone();
			result = nativeConnect(mNativePtr,
				mCtrlBlock.getVenderId(),
				mCtrlBlock.getProductId(),
				mCtrlBlock.getFileDescriptor(),
				mCtrlBlock.getBusNum(),
				mCtrlBlock.getDevNum(),
				getUSBFSName(mCtrlBlock));
		} catch (final Exception e) {
			Timber.e(e);
			result = -1;
		}

		if (result != 0) {
			throw new UnsupportedOperationException("open failed:result=" + result+"----->" +
					"id_camera="+mNativePtr+";venderId="+(mCtrlBlock==null ? "": mCtrlBlock.getVenderId())
					+";productId="+(mCtrlBlock==null ? "": mCtrlBlock.getProductId())+";fileDescriptor="+(mCtrlBlock==null ? "": mCtrlBlock.getFileDescriptor())
					+";busNum="+(mCtrlBlock==null ? "": mCtrlBlock.getBusNum())+";devAddr="+(mCtrlBlock==null ? "": mCtrlBlock.getDevNum())
					+";usbfs="+(mCtrlBlock==null ? "": getUSBFSName(mCtrlBlock)));
		}
		mCurrentFrameFormat = UvcFrameFormat.FRAME_FORMAT_MJPEG;
    	if (mNativePtr != 0 && mSupportedSize.isEmpty()) {
			getSupportedSize();
			Timber.d("Camera supported formats %s", mSupportedSize.toString());
    	}
    	if (USBMonitor.DEBUG) {
    		Timber.i("open camera status: " + mNativePtr +", size: " + mSupportedSize);
		}
    	var supportedSizes = getSupportedSizeList2();
		if (!supportedSizes.isEmpty()) {
			mCurrentWidth = supportedSizes.get(0).getWidth();
			mCurrentHeight = supportedSizes.get(0).getHeight();
		}
    }

    /**
     * close and release UVC camera
     */
	@Override
    public synchronized void close() {
    	stopCapturing();
    	if (mNativePtr != 0) {
    		nativeRelease(mNativePtr);
//    		mNativePtr = 0;	// nativeDestroyを呼ぶのでここでクリアしちゃダメ
    	}
    	if (mCtrlBlock != null) {
			mCtrlBlock.close();
   			mCtrlBlock = null;
		}
		mControlSupports = mProcSupports = 0;
		mCurrentFrameFormat = UvcFrameFormat.FRAME_FORMAT_NONE;
		mCurrentBandwidthFactor = 0;
		mSupportedSize = Collections.emptyMap();
    	Timber.v("close:finished");
    }

	public UsbDevice getDevice() {
		return mCtrlBlock != null ? mCtrlBlock.getDevice() : null;
	}

	public String getDeviceName(){
		return mCtrlBlock != null ? mCtrlBlock.getDeviceName() : null;
	}

	public UsbControlBlock getUsbControlBlock() {
		return mCtrlBlock;
	}

	public synchronized Map<UvcVsDeskSubtype, List<UvcCameraResolution>> getSupportedSize() {
		if (mSupportedSize.isEmpty()) {
			HashMap<UvcVsDeskSubtype, List<UvcCameraResolution>> result = new HashMap<>();
			var sizes = nativeGetSupportedSize(mNativePtr);
			for (var subtype: sizes.keySet()) {
				var subtypeValue = UvcVsDeskSubtype.UVC_VS_UNDEFINED;
				if (subtype == UvcVsDeskSubtype.UVC_VS_FORMAT_UNCOMPRESSED.getValue()) {
					subtypeValue = UvcVsDeskSubtype.UVC_VS_FORMAT_UNCOMPRESSED;
				}
				if (subtype == UvcVsDeskSubtype.UVC_VS_FORMAT_MJPEG.getValue()) {
					subtypeValue = UvcVsDeskSubtype.UVC_VS_FORMAT_MJPEG;
				}
				if (subtypeValue == UvcVsDeskSubtype.UVC_VS_UNDEFINED) {
					continue;	// skip unsupported subtype
				}
				result.put(subtypeValue, sizes.get(subtype));
			}
			mSupportedSize = result;
		}
    	return mSupportedSize;
    }

	/**
	 * Set preview size and preview mode
	 * @param width
	   @param height
	 */
	public void setPreviewSize(final int width, final int height) {
		setPreviewSize(width, height, DEFAULT_PREVIEW_FPS, mCurrentFrameFormat, mCurrentBandwidthFactor);
	}

	/**
	 * Set preview size and preview mode
	 * @param width
	 * @param height
	 * @param frameFormat either FRAME_FORMAT_YUYV(0) or FRAME_FORMAT_MJPEG(1)
	 */
	public void setPreviewSize(final int width, final int height, final UvcFrameFormat frameFormat) {
		setPreviewSize(width, height, DEFAULT_PREVIEW_FPS, frameFormat, mCurrentBandwidthFactor);
	}

	/**
	 * Set preview size and preview mode
	 * @param width
	   @param height
	   @param frameFormat either FRAME_FORMAT_YUYV(0) or FRAME_FORMAT_MJPEG(1)
	   @param bandwidth [0.0f,1.0f]
	 */
	public void setPreviewSize(final int width, final int height, final UvcFrameFormat frameFormat, final float bandwidth) {
		setPreviewSize(width, height, DEFAULT_PREVIEW_FPS, frameFormat, bandwidth);
	}

	/**
	 * Set preview size and preview mode
	 * @param width
	 * @param height
	 * @param fps 0 - any fps
	 * @param frameFormat either FRAME_FORMAT_YUYV(0) or FRAME_FORMAT_MJPEG(1)
	 * @param bandwidthFactor
	 */
	public void setPreviewSize(final int width,
							   final int height,
							   final int fps,
							   final UvcFrameFormat frameFormat,
							   final float bandwidthFactor) {
		if ((width == 0) || (height == 0))
			throw new IllegalArgumentException("invalid preview size");
		if (mNativePtr != 0) {
			final int result = nativeSetPreviewSize(mNativePtr, width, height, fps,
					frameFormat.getValue(), bandwidthFactor);
			if (result != 0)
				throw new IllegalArgumentException("Failed to set preview size " + width + "x" + height);
			mCurrentFrameFormat = frameFormat;
			mCurrentWidth = width;
			mCurrentHeight = height;
			mCurrentBandwidthFactor = bandwidthFactor;
		}
	}

	public List<UvcCameraResolution> getSupportedSizeList2() {
		return getSupportedSize2((mCurrentFrameFormat != UvcFrameFormat.FRAME_FORMAT_YUYV) ?
				UvcVsDeskSubtype.UVC_VS_FORMAT_MJPEG : UvcVsDeskSubtype.UVC_VS_FORMAT_UNCOMPRESSED,
				getSupportedSize());
	}

	public List<UvcCameraResolution> getSupportedSizeList2(UvcFrameFormat frameFormat) {
		return getSupportedSize2((frameFormat != UvcFrameFormat.FRAME_FORMAT_YUYV) ?
				UvcVsDeskSubtype.UVC_VS_FORMAT_MJPEG : UvcVsDeskSubtype.UVC_VS_FORMAT_UNCOMPRESSED,
				getSupportedSize());
	}

	public List<UvcCameraResolution> getSupportedSize2(final UvcVsDeskSubtype type, final Map<UvcVsDeskSubtype, List<UvcCameraResolution>> supportedSize) {
		return supportedSize.getOrDefault(type, Collections.emptyList());
	}

    /**
     * set preview surface with SurfaceHolder</br>
     * you can use SurfaceHolder came from SurfaceView/GLSurfaceView
     * @param holder
     */
    public synchronized void setPreviewDisplay(final SurfaceHolder holder) {
   		nativeSetPreviewDisplay(mNativePtr, holder.getSurface());
    }

    /**
     * set preview surface with SurfaceTexture.
     * this method require API >= 14
     */
    public synchronized void setPreviewTexture(final SurfaceTexture texture) {	// API >= 11
    	final Surface surface = new Surface(texture);	// XXX API >= 14
    	nativeSetPreviewDisplay(mNativePtr, surface);
    }

    /**
     * set preview surface with Surface
     */
    public synchronized void setPreviewDisplay(final Surface surface) {
    	nativeSetPreviewDisplay(mNativePtr, surface);
    }

    /**
     * set frame callback
     */
    public void setFrameCallback(final IFrameCallback callback, final PixelFormat pixelFormat) {
    	if (mNativePtr != 0) {
        	nativeSetFrameCallback(mNativePtr, callback, pixelFormat.getValue());
    	}
    }

    /**
     * start preview
     */
    public synchronized void startPreview() {
    	if (mCtrlBlock != null) {
    		nativeStartPreview(mNativePtr);
    	}
    }

	@Override
    public synchronized void stopCapturing() {
    	setFrameCallback(null, PixelFormat.PIXEL_FORMAT_RAW);
    	if (mCtrlBlock != null) {
    		nativeStopPreview(mNativePtr);
    	}
    }

    /**
     * destroy UVCCamera object
     */
    public synchronized void destroy() {
    	close();
    	if (mNativePtr != 0) {
    		nativeDestroy(mNativePtr);
    		mNativePtr = 0;
    	}
    }

//================================================================================
	public synchronized void setAutoFocus(final boolean autoFocus) {
    	if (mNativePtr != 0) {
    		nativeSetAutoFocus(mNativePtr, autoFocus);
    	}
    }

	public synchronized boolean getAutoFocus() {
    	boolean result = true;
    	if (mNativePtr != 0) {
    		result = nativeGetAutoFocus(mNativePtr) > 0;
    	}
    	return result;
    }
//================================================================================
    /**
     * @param focus [%]
     */
	public synchronized void setFocus(final int focus) {
    	if (mNativePtr != 0) {
 		   final float range = Math.abs(mFocusMax - mFocusMin);
 		   if (range > 0)
 			   nativeSetFocus(mNativePtr, (int)(focus / 100.f * range) + mFocusMin);
    	}
    }

    /**
     * @param focus_abs
     * @return focus[%]
     */
	public synchronized int getFocus(final int focus_abs) {
	   int result = 0;
	   if (mNativePtr != 0) {
		   nativeUpdateFocusLimit(mNativePtr);
		   final float range = Math.abs(mFocusMax - mFocusMin);
		   if (range > 0) {
			   result = (int)((focus_abs - mFocusMin) * 100.f / range);
		   }
	   }
	   return result;
	}

    /**
     * @return focus[%]
     */
	public synchronized int getFocus() {
    	return getFocus(nativeGetFocus(mNativePtr));
    }

	public synchronized void resetFocus() {
    	if (mNativePtr != 0) {
    		nativeSetFocus(mNativePtr, mFocusDef);
    	}
    }

//================================================================================
	public synchronized void setAutoWhiteBlance(final boolean autoWhiteBlance) {
    	if (mNativePtr != 0) {
    		nativeSetAutoWhiteBlance(mNativePtr, autoWhiteBlance);
    	}
    }

	public synchronized boolean getAutoWhiteBlance() {
    	boolean result = true;
    	if (mNativePtr != 0) {
    		result = nativeGetAutoWhiteBlance(mNativePtr) > 0;
    	}
    	return result;
    }

//================================================================================
    /**
     * @param whiteBlance [%]
     */
	public synchronized void setWhiteBlance(final int whiteBlance) {
    	if (mNativePtr != 0) {
 		   final float range = Math.abs(mWhiteBlanceMax - mWhiteBlanceMin);
 		   if (range > 0)
 			   nativeSetWhiteBlance(mNativePtr, (int)(whiteBlance / 100.f * range) + mWhiteBlanceMin);
    	}
    }

    /**
     * @param whiteBlance_abs
     * @return whiteBlance[%]
     */
	public synchronized int getWhiteBlance(final int whiteBlance_abs) {
	   int result = 0;
	   if (mNativePtr != 0) {
		   nativeUpdateWhiteBlanceLimit(mNativePtr);
		   final float range = Math.abs(mWhiteBlanceMax - mWhiteBlanceMin);
		   if (range > 0) {
			   result = (int)((whiteBlance_abs - mWhiteBlanceMin) * 100.f / range);
		   }
	   }
	   return result;
	}

    /**
     * @return white blance[%]
     */
	public synchronized int getWhiteBlance() {
    	return getWhiteBlance(nativeGetWhiteBlance(mNativePtr));
    }

	public synchronized void resetWhiteBlance() {
    	if (mNativePtr != 0) {
    		nativeSetWhiteBlance(mNativePtr, mWhiteBlanceDef);
    	}
    }
//================================================================================
    /**
     * @param brightness [%]
     */
	public synchronized void setBrightness(final int brightness) {
    	if (mNativePtr != 0) {
 		   final float range = Math.abs(mBrightnessMax - mBrightnessMin);
			Timber.d("setBrightness " + mBrightnessMax + " " + mBrightnessMin + " " + brightness);
 		   if (range > 0)
 			   nativeSetBrightness(mNativePtr, (int)(brightness / 100.f * range) + mBrightnessMin);
    	}
    }

    /**
     * @param brightness_abs
     * @return brightness[%]
     */
	public synchronized int getBrightness(final int brightness_abs) {
	   int result = 0;
	   if (mNativePtr != 0) {
		   nativeUpdateBrightnessLimit(mNativePtr);
		   final float range = Math.abs(mBrightnessMax - mBrightnessMin);
		   if (range > 0) {
			   result = (int)((brightness_abs - mBrightnessMin) * 100.f / range);
		   }
	   }
	   return result;
	}

    /**
     * @return brightness[%]
     */
	public synchronized int getBrightness() {
    	return getBrightness(nativeGetBrightness(mNativePtr));
    }

	public synchronized void resetBrightness() {
    	if (mNativePtr != 0) {
    		nativeSetBrightness(mNativePtr, mBrightnessDef);
    	}
    }

	public synchronized int getBrightnessMax() {
		return mBrightnessMax;
	}

	public synchronized int getBrightnessMin() {
		return mBrightnessMin;
	}

//================================================================================
    /**
     * @param contrast [%]
     */
	public synchronized void setContrast(final int contrast) {
    	if (mNativePtr != 0) {
    		nativeUpdateContrastLimit(mNativePtr);
	    	final float range = Math.abs(mContrastMax - mContrastMin);
	    	if (range > 0)
	    		nativeSetContrast(mNativePtr, (int)(contrast / 100.f * range) + mContrastMin);
    	}
    }

    /**
     * @param contrast_abs
     * @return contrast[%]
     */
	public synchronized int getContrast(final int contrast_abs) {
	   int result = 0;
	   if (mNativePtr != 0) {
		   final float range = Math.abs(mContrastMax - mContrastMin);
		   if (range > 0) {
			   result = (int)((contrast_abs - mContrastMin) * 100.f / range);
		   }
	   }
	   return result;
	}

    /**
     * @return contrast[%]
     */
	public synchronized int getContrast() {
    	return getContrast(nativeGetContrast(mNativePtr));
    }

	public synchronized void resetContrast() {
    	if (mNativePtr != 0) {
    		nativeSetContrast(mNativePtr, mContrastDef);
    	}
    }

//================================================================================
    /**
     * @param sharpness [%]
     */
	public synchronized void setSharpness(final int sharpness) {
    	if (mNativePtr != 0) {
 		   final float range = Math.abs(mSharpnessMax - mSharpnessMin);
 		   if (range > 0)
 			   nativeSetSharpness(mNativePtr, (int)(sharpness / 100.f * range) + mSharpnessMin);
    	}
    }

    /**
     * @param sharpness_abs
     * @return sharpness[%]
     */
	public synchronized int getSharpness(final int sharpness_abs) {
	   int result = 0;
	   if (mNativePtr != 0) {
		   nativeUpdateSharpnessLimit(mNativePtr);
		   final float range = Math.abs(mSharpnessMax - mSharpnessMin);
		   if (range > 0) {
			   result = (int)((sharpness_abs - mSharpnessMin) * 100.f / range);
		   }
	   }
	   return result;
	}

    /**
     * @return sharpness[%]
     */
	public synchronized int getSharpness() {
    	return getSharpness(nativeGetSharpness(mNativePtr));
    }

	public synchronized void resetSharpness() {
    	if (mNativePtr != 0) {
    		nativeSetSharpness(mNativePtr, mSharpnessDef);
    	}
    }
//================================================================================
    /**
     * @param gain [%]
     */
	public synchronized void setGain(final int gain) {
    	if (mNativePtr != 0) {
 		   final float range = Math.abs(mGainMax - mGainMin);
 		   if (range > 0)
 			   nativeSetGain(mNativePtr, (int)(gain / 100.f * range) + mGainMin);
    	}
    }

    /**
     * @param gain_abs
     * @return gain[%]
     */
	public synchronized int getGain(final int gain_abs) {
	   int result = 0;
	   if (mNativePtr != 0) {
		   nativeUpdateGainLimit(mNativePtr);
		   final float range = Math.abs(mGainMax - mGainMin);
		   if (range > 0) {
			   result = (int)((gain_abs - mGainMin) * 100.f / range);
		   }
	   }
	   return result;
	}

    /**
     * @return gain[%]
     */
	public synchronized int getGain() {
    	return getGain(nativeGetGain(mNativePtr));
    }

	public synchronized void resetGain() {
    	if (mNativePtr != 0) {
    		nativeSetGain(mNativePtr, mGainDef);
    	}
    }

//================================================================================
    /**
     * @param gamma [%]
     */
	public synchronized void setGamma(final int gamma) {
    	if (mNativePtr != 0) {
 		   final float range = Math.abs(mGammaMax - mGammaMin);
 		   if (range > 0)
 			   nativeSetGamma(mNativePtr, (int)(gamma / 100.f * range) + mGammaMin);
    	}
    }

    /**
     * @param gamma_abs
     * @return gamma[%]
     */
	public synchronized int getGamma(final int gamma_abs) {
	   int result = 0;
	   if (mNativePtr != 0) {
		   nativeUpdateGammaLimit(mNativePtr);
		   final float range = Math.abs(mGammaMax - mGammaMin);
		   if (range > 0) {
			   result = (int)((gamma_abs - mGammaMin) * 100.f / range);
		   }
	   }
	   return result;
	}

    /**
     * @return gamma[%]
     */
	public synchronized int getGamma() {
    	return getGamma(nativeGetGamma(mNativePtr));
    }

	public synchronized void resetGamma() {
    	if (mNativePtr != 0) {
    		nativeSetGamma(mNativePtr, mGammaDef);
    	}
    }

//================================================================================
    /**
     * @param saturation [%]
     */
	public synchronized void setSaturation(final int saturation) {
    	if (mNativePtr != 0) {
 		   final float range = Math.abs(mSaturationMax - mSaturationMin);
 		   if (range > 0)
 			   nativeSetSaturation(mNativePtr, (int)(saturation / 100.f * range) + mSaturationMin);
    	}
    }

    /**
     * @param saturation_abs
     * @return saturation[%]
     */
	public synchronized int getSaturation(final int saturation_abs) {
	   int result = 0;
	   if (mNativePtr != 0) {
		   nativeUpdateSaturationLimit(mNativePtr);
		   final float range = Math.abs(mSaturationMax - mSaturationMin);
		   if (range > 0) {
			   result = (int)((saturation_abs - mSaturationMin) * 100.f / range);
		   }
	   }
	   return result;
	}

    /**
     * @return saturation[%]
     */
	public synchronized int getSaturation() {
    	return getSaturation(nativeGetSaturation(mNativePtr));
    }

	public synchronized void resetSaturation() {
    	if (mNativePtr != 0) {
    		nativeSetSaturation(mNativePtr, mSaturationDef);
    	}
    }
//================================================================================
    /**
     * @param hue [%]
     */
	public synchronized void setHue(final int hue) {
    	if (mNativePtr != 0) {
 		   final float range = Math.abs(mHueMax - mHueMin);
 		   if (range > 0)
 			   nativeSetHue(mNativePtr, (int)(hue / 100.f * range) + mHueMin);
    	}
    }

    /**
     * @param hue_abs
     * @return hue[%]
     */
	public synchronized int getHue(final int hue_abs) {
	   int result = 0;
	   if (mNativePtr != 0) {
		   nativeUpdateHueLimit(mNativePtr);
		   final float range = Math.abs(mHueMax - mHueMin);
		   if (range > 0) {
			   result = (int)((hue_abs - mHueMin) * 100.f / range);
		   }
	   }
	   return result;
	}

    /**
     * @return hue[%]
     */
	public synchronized int getHue() {
    	return getHue(nativeGetHue(mNativePtr));
    }

	public synchronized void resetHue() {
    	if (mNativePtr != 0) {
    		nativeSetHue(mNativePtr, mSaturationDef);
    	}
    }

//================================================================================
	public void setPowerlineFrequency(final int frequency) {
    	if (mNativePtr != 0)
    		nativeSetPowerlineFrequency(mNativePtr, frequency);
    }

	public int getPowerlineFrequency() {
    	return nativeGetPowerlineFrequency(mNativePtr);
    }

//================================================================================
    /**
     * this may not work well with some combination of camera and device
     * @param zoom [%]
     */
	public synchronized void setZoom(final int zoom) {
    	if (mNativePtr != 0) {
 		   final float range = Math.abs(mZoomMax - mZoomMin);
 		   if (range > 0) {
 			   final int z = (int)(zoom / 100.f * range) + mZoomMin;
// 			   XLogWrapper.d(TAG, "setZoom:zoom=" + zoom + " ,value=" + z);
 			   nativeSetZoom(mNativePtr, z);
 		   }
    	}
    }

    /**
     * @param zoom_abs
     * @return zoom[%]
     */
	public synchronized int getZoom(final int zoom_abs) {
	   int result = 0;
	   if (mNativePtr != 0) {
		   nativeUpdateZoomLimit(mNativePtr);
		   final float range = Math.abs(mZoomMax - mZoomMin);
		   if (range > 0) {
			   result = (int)((zoom_abs - mZoomMin) * 100.f / range);
		   }
	   }
	   return result;
	}

    /**
     * @return zoom[%]
     */
	public synchronized int getZoom() {
    	return getZoom(nativeGetZoom(mNativePtr));
    }

	public synchronized void resetZoom() {
    	if (mNativePtr != 0) {
    		nativeSetZoom(mNativePtr, mZoomDef);
    	}
    }

//================================================================================

	/** Command camera
	 *
	 * @param command hex value
	 * @return control result
	 */
	public synchronized int sendCommand(int command) {
		if (mNativePtr != 0) {
			return nativeSendCommand(mNativePtr, command);
		}
		return (int) mNativePtr;
	}

//================================================================================
	public synchronized void updateCameraParams() {
    	if (mNativePtr != 0) {
    		if ((mControlSupports == 0) || (mProcSupports == 0)) {
        		// サポートしている機能フラグを取得
    			if (mControlSupports == 0)
    				mControlSupports = nativeGetCtrlSupports(mNativePtr);
    			if (mProcSupports == 0)
    				mProcSupports = nativeGetProcSupports(mNativePtr);
    	    	// 設定値を取得
    	    	if ((mControlSupports != 0) && (mProcSupports != 0)) {
	    	    	nativeUpdateBrightnessLimit(mNativePtr);
	    	    	nativeUpdateContrastLimit(mNativePtr);
	    	    	nativeUpdateSharpnessLimit(mNativePtr);
	    	    	nativeUpdateGainLimit(mNativePtr);
	    	    	nativeUpdateGammaLimit(mNativePtr);
	    	    	nativeUpdateSaturationLimit(mNativePtr);
	    	    	nativeUpdateHueLimit(mNativePtr);
	    	    	nativeUpdateZoomLimit(mNativePtr);
	    	    	nativeUpdateWhiteBlanceLimit(mNativePtr);
	    	    	nativeUpdateFocusLimit(mNativePtr);
    	    	}
    	    	if (false) {
					dumpControls(mControlSupports);
					dumpProc(mProcSupports);
					Timber.v(String.format("Brightness:min=%d,max=%d,def=%d", mBrightnessMin, mBrightnessMax, mBrightnessDef));
					Timber.v(String.format("Contrast:min=%d,max=%d,def=%d", mContrastMin, mContrastMax, mContrastDef));
					Timber.v(String.format("Sharpness:min=%d,max=%d,def=%d", mSharpnessMin, mSharpnessMax, mSharpnessDef));
					Timber.v(String.format("Gain:min=%d,max=%d,def=%d", mGainMin, mGainMax, mGainDef));
					Timber.v(String.format("Gamma:min=%d,max=%d,def=%d", mGammaMin, mGammaMax, mGammaDef));
					Timber.v(String.format("Saturation:min=%d,max=%d,def=%d", mSaturationMin, mSaturationMax, mSaturationDef));
					Timber.v(String.format("Hue:min=%d,max=%d,def=%d", mHueMin, mHueMax, mHueDef));
					Timber.v(String.format("Zoom:min=%d,max=%d,def=%d", mZoomMin, mZoomMax, mZoomDef));
					Timber.v(String.format("WhiteBlance:min=%d,max=%d,def=%d", mWhiteBlanceMin, mWhiteBlanceMax, mWhiteBlanceDef));
					Timber.v(String.format("Focus:min=%d,max=%d,def=%d", mFocusMin, mFocusMax, mFocusDef));
				}
			}
    	} else {
    		mControlSupports = mProcSupports = 0;
    	}
    }

    private static final String[] SUPPORTS_CTRL = {
    	"D0:  Scanning Mode",
    	"D1:  Auto-Exposure Mode",
    	"D2:  Auto-Exposure Priority",
    	"D3:  Exposure Time (Absolute)",
    	"D4:  Exposure Time (Relative)",
    	"D5:  Focus (Absolute)",
    	"D6:  Focus (Relative)",
    	"D7:  Iris (Absolute)",
    	"D8:  Iris (Relative)",
    	"D9:  Zoom (Absolute)",
    	"D10: Zoom (Relative)",
    	"D11: PanTilt (Absolute)",
    	"D12: PanTilt (Relative)",
    	"D13: Roll (Absolute)",
    	"D14: Roll (Relative)",
		"D15: Reserved",
		"D16: Reserved",
		"D17: Focus, Auto",
		"D18: Privacy",
		"D19: Focus, Simple",
		"D20: Window",
		"D21: Region of Interest",
		"D22: Reserved, set to zero",
		"D23: Reserved, set to zero",
    };

    private static final String[] SUPPORTS_PROC = {
		"D0: Brightness",
		"D1: Contrast",
		"D2: Hue",
		"D3: Saturation",
		"D4: Sharpness",
		"D5: Gamma",
		"D6: White Balance Temperature",
		"D7: White Balance Component",
		"D8: Backlight Compensation",
		"D9: Gain",
		"D10: Power Line Frequency",
		"D11: Hue, Auto",
		"D12: White Balance Temperature, Auto",
		"D13: White Balance Component, Auto",
		"D14: Digital Multiplier",
		"D15: Digital Multiplier Limit",
		"D16: AnaXLogWrapper Video Standard",
		"D17: AnaXLogWrapper Video Lock Status",
		"D18: Contrast, Auto",
		"D19: Reserved. Set to zero",
		"D20: Reserved. Set to zero",
		"D21: Reserved. Set to zero",
		"D22: Reserved. Set to zero",
		"D23: Reserved. Set to zero",
	};

    private static final void dumpControls(final long controlSupports) {
    	Timber.i(String.format("controlSupports=%x", controlSupports));
    	for (int i = 0; i < SUPPORTS_CTRL.length; i++) {
    		Timber.i(SUPPORTS_CTRL[i] + ((controlSupports & (0x1 << i)) != 0 ? "=enabled" : "=disabled"));
    	}
    }

	private static final void dumpProc(final long procSupports) {
    	Timber.i(String.format("procSupports=%x", procSupports));
    	for (int i = 0; i < SUPPORTS_PROC.length; i++) {
    		Timber.i(SUPPORTS_PROC[i] + ((procSupports & (0x1 << i)) != 0 ? "=enabled" : "=disabled"));
    	}
    }

	private String getUSBFSName(final com.jiangdg.usb.UsbControlBlock ctrlBlock) {
		String result = null;
		final String name = ctrlBlock.getDeviceName();
		final String[] v = !TextUtils.isEmpty(name) ? name.split("/") : null;
		if ((v != null) && (v.length > 2)) {
			final StringBuilder sb = new StringBuilder(v[0]);
			for (int i = 1; i < v.length - 2; i++)
				sb.append("/").append(v[i]);
			result = sb.toString();
		}
		if (TextUtils.isEmpty(result)) {
			Timber.w("failed to get USBFS path, try to use default path:" + DEFAULT_USBFS);
			result = DEFAULT_USBFS;
		}
		return name;
	}

	// #nativeCreate and #nativeDestroy are not static methods.
	private final native long nativeCreate();
	private final native void nativeDestroy(final long id_camera);

	private native int nativeConnect(long id_camera,
                                     int venderId,
                                     int productId,
                                     int fileDescriptor,
                                     int busNum,
                                     int devAddr,
                                     String usbfs);
	private static final native int nativeRelease(final long id_camera);

	private static final native int nativeSetStatusCallback(final long mNativePtr, final IStatusCallback callback);
	private static final native int nativeSetButtonCallback(final long mNativePtr, final IButtonCallback callback);

	private static native int nativeSetPreviewSize(final long id_camera,
                                                   final int width,
                                                   final int height,
                                                   final float fps,
                                                   final int mode,
                                                   final float bandwidth);
	private static native Map<Integer, List<UvcCameraResolution>> nativeGetSupportedSize(final long id_camera);
	private static final native int nativeStartPreview(final long id_camera);
	private static final native int nativeStopPreview(final long id_camera);
	private static final native int nativeSetPreviewDisplay(final long id_camera, final Surface surface);
	private static final native int nativeSetFrameCallback(final long mNativePtr, final IFrameCallback callback, final int pixelFormat);

//**********************************************************************

	private static final native long nativeGetCtrlSupports(final long id_camera);
	private static final native long nativeGetProcSupports(final long id_camera);

	private final native int nativeUpdateScanningModeLimit(final long id_camera);
	private static final native int nativeSetScanningMode(final long id_camera, final int scanning_mode);
	private static final native int nativeGetScanningMode(final long id_camera);

	private final native int nativeUpdateExposureModeLimit(final long id_camera);
	private static final native int nativeSetExposureMode(final long id_camera, final int exposureMode);
	private static final native int nativeGetExposureMode(final long id_camera);

	private final native int nativeUpdateExposurePriorityLimit(final long id_camera);
	private static final native int nativeSetExposurePriority(final long id_camera, final int priority);
	private static final native int nativeGetExposurePriority(final long id_camera);

	private final native int nativeUpdateExposureLimit(final long id_camera);
	private static final native int nativeSetExposure(final long id_camera, final int exposure);
	private static final native int nativeGetExposure(final long id_camera);

	private final native int nativeUpdateExposureRelLimit(final long id_camera);
	private static final native int nativeSetExposureRel(final long id_camera, final int exposure_rel);
	private static final native int nativeGetExposureRel(final long id_camera);

	private final native int nativeUpdateAutoFocusLimit(final long id_camera);
	private static final native int nativeSetAutoFocus(final long id_camera, final boolean autofocus);
	private static final native int nativeGetAutoFocus(final long id_camera);

	private final native int nativeUpdateFocusLimit(final long id_camera);
	private static final native int nativeSetFocus(final long id_camera, final int focus);
	private static final native int nativeGetFocus(final long id_camera);

	private final native int nativeUpdateFocusRelLimit(final long id_camera);
	private static final native int nativeSetFocusRel(final long id_camera, final int focus_rel);
	private static final native int nativeGetFocusRel(final long id_camera);

	private final native int nativeUpdateIrisLimit(final long id_camera);
	private static final native int nativeSetIris(final long id_camera, final int iris);
	private static final native int nativeGetIris(final long id_camera);

	private final native int nativeUpdateIrisRelLimit(final long id_camera);
	private static final native int nativeSetIrisRel(final long id_camera, final int iris_rel);
	private static final native int nativeGetIrisRel(final long id_camera);

	private final native int nativeUpdatePanLimit(final long id_camera);
	private static final native int nativeSetPan(final long id_camera, final int pan);
	private static final native int nativeGetPan(final long id_camera);

	private final native int nativeUpdatePanRelLimit(final long id_camera);
	private static final native int nativeSetPanRel(final long id_camera, final int pan_rel);
	private static final native int nativeGetPanRel(final long id_camera);

	private final native int nativeUpdateTiltLimit(final long id_camera);
	private static final native int nativeSetTilt(final long id_camera, final int tilt);
	private static final native int nativeGetTilt(final long id_camera);

	private final native int nativeUpdateTiltRelLimit(final long id_camera);
	private static final native int nativeSetTiltRel(final long id_camera, final int tilt_rel);
	private static final native int nativeGetTiltRel(final long id_camera);

	private final native int nativeUpdateRollLimit(final long id_camera);
	private static final native int nativeSetRoll(final long id_camera, final int roll);
	private static final native int nativeGetRoll(final long id_camera);

	private final native int nativeUpdateRollRelLimit(final long id_camera);
	private static final native int nativeSetRollRel(final long id_camera, final int roll_rel);
	private static final native int nativeGetRollRel(final long id_camera);

	private final native int nativeUpdateAutoWhiteBlanceLimit(final long id_camera);
	private static final native int nativeSetAutoWhiteBlance(final long id_camera, final boolean autoWhiteBlance);
	private static final native int nativeGetAutoWhiteBlance(final long id_camera);

	private final native int nativeUpdateAutoWhiteBlanceCompoLimit(final long id_camera);
	private static final native int nativeSetAutoWhiteBlanceCompo(final long id_camera, final boolean autoWhiteBlanceCompo);
	private static final native int nativeGetAutoWhiteBlanceCompo(final long id_camera);

	private final native int nativeUpdateWhiteBlanceLimit(final long id_camera);
	private static final native int nativeSetWhiteBlance(final long id_camera, final int whiteBlance);
	private static final native int nativeGetWhiteBlance(final long id_camera);

	private final native int nativeUpdateWhiteBlanceCompoLimit(final long id_camera);
	private static final native int nativeSetWhiteBlanceCompo(final long id_camera, final int whiteBlance_compo);
	private static final native int nativeGetWhiteBlanceCompo(final long id_camera);

	private final native int nativeUpdateBacklightCompLimit(final long id_camera);
	private static final native int nativeSetBacklightComp(final long id_camera, final int backlight_comp);
	private static final native int nativeGetBacklightComp(final long id_camera);

	private final native int nativeUpdateBrightnessLimit(final long id_camera);
	private static final native int nativeSetBrightness(final long id_camera, final int brightness);
	private static final native int nativeGetBrightness(final long id_camera);

	private final native int nativeUpdateContrastLimit(final long id_camera);
	private static final native int nativeSetContrast(final long id_camera, final int contrast);
	private static final native int nativeGetContrast(final long id_camera);

	private final native int nativeUpdateAutoContrastLimit(final long id_camera);
	private static final native int nativeSetAutoContrast(final long id_camera, final boolean autocontrast);
	private static final native int nativeGetAutoContrast(final long id_camera);

	private final native int nativeUpdateSharpnessLimit(final long id_camera);
	private static final native int nativeSetSharpness(final long id_camera, final int sharpness);
	private static final native int nativeGetSharpness(final long id_camera);

	private final native int nativeUpdateGainLimit(final long id_camera);
	private static final native int nativeSetGain(final long id_camera, final int gain);
	private static final native int nativeGetGain(final long id_camera);

	private final native int nativeUpdateGammaLimit(final long id_camera);
	private static final native int nativeSetGamma(final long id_camera, final int gamma);
	private static final native int nativeGetGamma(final long id_camera);

	private final native int nativeUpdateSaturationLimit(final long id_camera);
	private static final native int nativeSetSaturation(final long id_camera, final int saturation);
	private static final native int nativeGetSaturation(final long id_camera);

	private final native int nativeUpdateHueLimit(final long id_camera);
	private static final native int nativeSetHue(final long id_camera, final int hue);
	private static final native int nativeGetHue(final long id_camera);

	private final native int nativeUpdateAutoHueLimit(final long id_camera);
	private static final native int nativeSetAutoHue(final long id_camera, final boolean autohue);
	private static final native int nativeGetAutoHue(final long id_camera);

	private final native int nativeUpdatePowerlineFrequencyLimit(final long id_camera);
	private static final native int nativeSetPowerlineFrequency(final long id_camera, final int frequency);
	private static final native int nativeGetPowerlineFrequency(final long id_camera);

	private static final native int nativeSendCommand(final long id_camera, final int command);

	private final native int nativeUpdateZoomLimit(final long id_camera);
	private static final native int nativeSetZoom(final long id_camera, final int zoom);
	private static final native int nativeGetZoom(final long id_camera);

	private final native int nativeUpdateZoomRelLimit(final long id_camera);
	private static final native int nativeSetZoomRel(final long id_camera, final int zoom_rel);
	private static final native int nativeGetZoomRel(final long id_camera);

	private final native int nativeUpdateDigitalMultiplierLimit(final long id_camera);
	private static final native int nativeSetDigitalMultiplier(final long id_camera, final int multiplier);
	private static final native int nativeGetDigitalMultiplier(final long id_camera);

	private final native int nativeUpdateDigitalMultiplierLimitLimit(final long id_camera);
	private static final native int nativeSetDigitalMultiplierLimit(final long id_camera, final int multiplier_limit);
	private static final native int nativeGetDigitalMultiplierLimit(final long id_camera);

	private final native int nativeUpdateAnalogVideoStandardLimit(final long id_camera);
	private static final native int nativeSetAnalogVideoStandard(final long id_camera, final int standard);
	private static final native int nativeGetAnalogVideoStandard(final long id_camera);

	private final native int nativeUpdateAnalogVideoLockStateLimit(final long id_camera);
	private static final native int nativeSetAnalogVideoLoackState(final long id_camera, final int state);
	private static final native int nativeGetAnalogVideoLoackState(final long id_camera);

	private final native int nativeUpdatePrivacyLimit(final long id_camera);
	private static final native int nativeSetPrivacy(final long id_camera, final boolean privacy);
	private static final native int nativeGetPrivacy(final long id_camera);
}
