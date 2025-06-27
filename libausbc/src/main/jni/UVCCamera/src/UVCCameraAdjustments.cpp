/*
 * UVCCameraConfig.cpp
 * library and sample to access to UVC web camera on non-rooted Android device
 *
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

#include "libuvc/libuvc.h"
#include "UVCCameraAdjustments.h"
#include "UVCConstants.h"
#include "utilbase.h"

//======================================================================
//#define CTRL_BRIGHTNESS        0
//#define CTRL_CONTRAST        1
//#define    CTRL_SHARPNESS        2
//#define CTRL_GAIN            3
//#define CTRL_WHITEBLANCE    4
//#define CTRL_FOCUS            5

static uvc_error_t update_ctrl_values(uvc_device_handle_t *devh,
                                      control_value_t &values,
                                      paramget_func_i16 get_func) {
    uvc_error_t ret = UVC_SUCCESS;
    if (!values.min && !values.max) {
        int16_t value;
        ret = get_func(devh, &value, UVC_GET_MIN);
        if (LIKELY(!ret)) {
            values.min = value;
            LOGV("update_params:min value=%d,min=%d", value, values.min);
            ret = get_func(devh, &value, UVC_GET_MAX);
            if (LIKELY(!ret)) {
                values.max = value;
                LOGV("update_params:max value=%d,max=%d", value, values.max);
                ret = get_func(devh, &value, UVC_GET_DEF);
                if (LIKELY(!ret)) {
                    values.def = value;
                    LOGV("update_params:def value=%d,def=%d", value, values.def);
                }
            }
        }
    }
    if (UNLIKELY(ret)) {
        LOGD("update_params failed:err=%d", ret);
    }
    RETURN(ret, uvc_error_t);
}

static uvc_error_t update_ctrl_values(uvc_device_handle_t *devh,
                                      control_value_t &values,
                                      paramget_func_u16 get_func) {
    uvc_error_t ret = UVC_SUCCESS;
    if (!values.min && !values.max) {
        uint16_t value;
        ret = get_func(devh, &value, UVC_GET_MIN);
        if (LIKELY(!ret)) {
            values.min = value;
            LOGV("update_params:min value=%d,min=%d", value, values.min);
            ret = get_func(devh, &value, UVC_GET_MAX);
            if (LIKELY(!ret)) {
                values.max = value;
                LOGV("update_params:max value=%d,max=%d", value, values.max);
                ret = get_func(devh, &value, UVC_GET_DEF);
                if (LIKELY(!ret)) {
                    values.def = value;
                    LOGV("update_params:def value=%d,def=%d", value, values.def);
                }
            }
        }
    }
    if (UNLIKELY(ret)) {
        LOGD("update_params failed:err=%d", ret);
    }
    RETURN(ret, uvc_error_t);
}

static uvc_error_t update_ctrl_values(uvc_device_handle_t *devh,
                                      control_value_t &values,
                                      paramget_func_i8 get_func) {
    uvc_error_t ret = UVC_SUCCESS;
    if (!values.min && !values.max) {
        int8_t value;
        ret = get_func(devh, &value, UVC_GET_MIN);
        if (LIKELY(!ret)) {
            values.min = value;
            LOGV("update_params:min value=%d,min=%d", value, values.min);
            ret = get_func(devh, &value, UVC_GET_MAX);
            if (LIKELY(!ret)) {
                values.max = value;
                LOGV("update_params:max value=%d,max=%d", value, values.max);
                ret = get_func(devh, &value, UVC_GET_DEF);
                if (LIKELY(!ret)) {
                    values.def = value;
                    LOGV("update_params:def value=%d,def=%d", value, values.def);
                }
            }
        }
    }
    if (UNLIKELY(ret)) {
        LOGD("update_params failed:err=%d", ret);
    }
    RETURN(ret, uvc_error_t);
}

static uvc_error_t update_ctrl_values(uvc_device_handle_t *devh, control_value_t &values,
                                      paramget_func_u8 get_func) {


    uvc_error_t ret = UVC_SUCCESS;
    if (!values.min && !values.max) {
        uint8_t value;
        ret = get_func(devh, &value, UVC_GET_MIN);
        if (LIKELY(!ret)) {
            values.min = value;
            LOGV("update_params:min value=%d,min=%d", value, values.min);
            ret = get_func(devh, &value, UVC_GET_MAX);
            if (LIKELY(!ret)) {
                values.max = value;
                LOGV("update_params:max value=%d,max=%d", value, values.max);
                ret = get_func(devh, &value, UVC_GET_DEF);
                if (LIKELY(!ret)) {
                    values.def = value;
                    LOGV("update_params:def value=%d,def=%d", value, values.def);
                }
            }
        }
    }
    if (UNLIKELY(ret)) {
        LOGD("update_params failed:err=%d", ret);
    }
    RETURN(ret, uvc_error_t);
}

static uvc_error_t update_ctrl_values(uvc_device_handle_t *devh, control_value_t &values,
                                      paramget_func_u8u8 get_func) {


    uvc_error_t ret = UVC_SUCCESS;
    if (!values.min && !values.max) {
        uint8_t value1, value2;
        ret = get_func(devh, &value1, &value2, UVC_GET_MIN);
        if (LIKELY(!ret)) {
            values.min = (value1 << 8) + value2;
            LOGV("update_params:min value1=%d,value2=%d,min=%d", value1, value2, values.min);
            ret = get_func(devh, &value1, &value2, UVC_GET_MAX);
            if (LIKELY(!ret)) {
                values.max = (value1 << 8) + value2;
                LOGV("update_params:max value1=%d,value2=%d,max=%d", value1, value2, values.max);
                ret = get_func(devh, &value1, &value2, UVC_GET_DEF);
                if (LIKELY(!ret)) {
                    values.def = (value1 << 8) + value2;
                    LOGV("update_params:def value1=%d,value2=%ddef=%d", value1, value2, values.def);
                }
            }
        }
    }
    if (UNLIKELY(ret)) {
        LOGD("update_params failed:err=%d", ret);
    }
    RETURN(ret, uvc_error_t);
}

static uvc_error_t update_ctrl_values(uvc_device_handle_t *devh, control_value_t &values,
                                      paramget_func_i8u8 get_func) {


    uvc_error_t ret = UVC_SUCCESS;
    if (!values.min && !values.max) {
        int8_t value1;
        uint8_t value2;
        ret = get_func(devh, &value1, &value2, UVC_GET_MIN);
        if (LIKELY(!ret)) {
            values.min = (value1 << 8) + value2;
            LOGV("update_params:min value1=%d,value2=%d,min=%d", value1, value2, values.min);
            ret = get_func(devh, &value1, &value2, UVC_GET_MAX);
            if (LIKELY(!ret)) {
                values.max = (value1 << 8) + value2;
                LOGV("update_params:max value1=%d,value2=%d,max=%d", value1, value2, values.max);
                ret = get_func(devh, &value1, &value2, UVC_GET_DEF);
                if (LIKELY(!ret)) {
                    values.def = (value1 << 8) + value2;
                    LOGV("update_params:def value1=%d,value2=%ddef=%d", value1, value2, values.def);
                }
            }
        }
    }
    if (UNLIKELY(ret)) {
        LOGD("update_params failed:err=%d", ret);
    }
    RETURN(ret, uvc_error_t);
}

static uvc_error_t update_ctrl_values(uvc_device_handle_t *devh, control_value_t &values,
                                      paramget_func_i8u8u8 get_func) {


    uvc_error_t ret = UVC_SUCCESS;
    if (!values.min && !values.max) {
        int8_t value1;
        uint8_t value2;
        uint8_t value3;
        ret = get_func(devh, &value1, &value2, &value3, UVC_GET_MIN);
        if (LIKELY(!ret)) {
            values.min = (value1 << 16) + (value2 << 8) + value3;
            LOGV("update_params:min value1=%d,value2=%d,value3=%d,min=%d", value1, value2, value3, values.min);
            ret = get_func(devh, &value1, &value2, &value3, UVC_GET_MAX);
            if (LIKELY(!ret)) {
                values.max = (value1 << 16) + (value2 << 8) + value3;
                LOGV("update_params:max value1=%d,value2=%d,value3=%d,max=%d", value1, value2, value3, values.max);
                ret = get_func(devh, &value1, &value2, &value3, UVC_GET_DEF);
                if (LIKELY(!ret)) {
                    values.def = (value1 << 16) + (value2 << 8) + value3;
                    LOGV("update_params:def value1=%d,value2=%d,value3=%d,def=%d", value1, value2, value3, values.def);
                }
            }
        }
    }
    if (UNLIKELY(ret)) {
        LOGD("update_params failed:err=%d", ret);
    }
    RETURN(ret, uvc_error_t);
}

static uvc_error_t update_ctrl_values(uvc_device_handle_t *devh, control_value_t &values,
                                      paramget_func_i32 get_func) {


    uvc_error_t ret = UVC_SUCCESS;
    if (!values.min && !values.max) {
        int32_t value;
        ret = get_func(devh, &value, UVC_GET_MIN);
        if (LIKELY(!ret)) {
            values.min = value;
            LOGV("update_params:min value=%d,min=%d", value, values.min);
            ret = get_func(devh, &value, UVC_GET_MAX);
            if (LIKELY(!ret)) {
                values.max = value;
                LOGV("update_params:max value=%d,max=%d", value, values.max);
                ret = get_func(devh, &value, UVC_GET_DEF);
                if (LIKELY(!ret)) {
                    values.def = value;
                    LOGV("update_params:def value=%d,def=%d", value, values.def);
                }
            }
        }
    }
    if (UNLIKELY(ret)) {
        LOGD("update_params failed:err=%d", ret);
    }
    RETURN(ret, uvc_error_t);
}

static uvc_error_t update_ctrl_values(uvc_device_handle_t *devh, control_value_t &values,
                                      paramget_func_u32 get_func) {


    uvc_error_t ret = UVC_SUCCESS;
    if (!values.min && !values.max) {
        uint32_t value;
        ret = get_func(devh, &value, UVC_GET_MIN);
        if (LIKELY(!ret)) {
            values.min = value;
            LOGV("update_params:min value=%d,min=%d", value, values.min);
            ret = get_func(devh, &value, UVC_GET_MAX);
            if (LIKELY(!ret)) {
                values.max = value;
                LOGV("update_params:max value=%d,max=%d", value, values.max);
                ret = get_func(devh, &value, UVC_GET_DEF);
                if (LIKELY(!ret)) {
                    values.def = value;
                    LOGV("update_params:def value=%d,def=%d", value, values.def);
                }
            }
        }
    }
    if (UNLIKELY(ret)) {
        LOGD("update_params failed:err=%d", ret);
    }
    RETURN(ret, uvc_error_t);
}

static uvc_error_t update_ctrl_values(uvc_device_handle_t *devh, control_value_t &values1, control_value_t &values2,
                                      paramget_func_i32i32 get_func) {


    uvc_error_t ret = UVC_SUCCESS;
    if ((!values1.min && !values1.max) || (!values2.min && !values2.max)) {
        int32_t value1, value2;
        ret = get_func(devh, &value1, &value2, UVC_GET_MIN);
        if (LIKELY(!ret)) {
            values1.min = value1;
            values2.min = value2;
            LOGV("update_params:min value1=%d,value2=%d", value1, value2);
            ret = get_func(devh, &value1, &value2, UVC_GET_MAX);
            if (LIKELY(!ret)) {
                values1.max = value1;
                values2.max = value2;
                LOGV("update_params:max value1=%d,value2=%d", value1, value2);
                ret = get_func(devh, &value1, &value2, UVC_GET_DEF);
                if (LIKELY(!ret)) {
                    values1.def = value1;
                    values2.def = value2;
                    LOGV("update_params:def value1=%d,value2=%d", value1, value2);
                }
            }
        }
    }
    if (UNLIKELY(ret)) {
        LOGD("update_params failed:err=%d", ret);
    }
    RETURN(ret, uvc_error_t);
}

#define UPDATE_CTRL_VALUES(VAL, FUNC) \
    ret = update_ctrl_values(mDeviceHandle, VAL, FUNC); \
    if (LIKELY(!ret)) { \
        min = VAL.min; \
        max = VAL.max; \
        def = VAL.def; \
    } else { \
        MARK("failed to UPDATE_CTRL_VALUES"); \
    } \


void UVCCameraAdjustments::clearCameraParams() {
    mCtrlSupports = 0;
    mPUSupports = 0;
    mScanningMode.min = mScanningMode.max = mScanningMode.def = 0;
    mExposureMode.min = mExposureMode.max = mExposureMode.def = 0;
    mExposurePriority.min = mExposurePriority.max = mExposurePriority.def = 0;
    mExposureAbs.min = mExposureAbs.max = mExposureAbs.def = 0;
    mAutoFocus.min = mAutoFocus.max = mAutoFocus.def = 0;
    mAutoWhiteBlance.min = mAutoWhiteBlance.max = mAutoWhiteBlance.def = 0;
    mWhiteBlance.min = mWhiteBlance.max = mWhiteBlance.def = 0;
    mAutoWhiteBlanceCompo.min = mAutoWhiteBlanceCompo.max = mAutoWhiteBlanceCompo.def = 0;
    mWhiteBlanceCompo.min = mWhiteBlanceCompo.max = mWhiteBlanceCompo.def = 0;
    mBacklightComp.min = mBacklightComp.max = mBacklightComp.def = 0;
    mBrightness.min = mBrightness.max = mBrightness.def = 0;
    mContrast.min = mContrast.max = mContrast.def = 0;
    mAutoContrast.min = mAutoContrast.max = mAutoContrast.def = 0;
    mSharpness.min = mSharpness.max = mSharpness.def = 0;
    mGain.min = mGain.max = mGain.def = 0;
    mGamma.min = mGamma.max = mGamma.def = 0;
    mSaturation.min = mSaturation.max = mSaturation.def = 0;
    mHue.min = mHue.max = mHue.def = 0;
    mAutoHue.min = mAutoHue.max = mAutoHue.def = 0;
    mZoom.min = mZoom.max = mZoom.def = 0;
    mZoomRel.min = mZoomRel.max = mZoomRel.def = 0;
    mFocus.min = mFocus.max = mFocus.def = 0;
    mFocusRel.min = mFocusRel.max = mFocusRel.def = 0;
    mFocusSimple.min = mFocusSimple.max = mFocusSimple.def = 0;
    mIris.min = mIris.max = mIris.def = 0;
    mIrisRel.min = mIrisRel.max = mIrisRel.def = 0;
    mPan.min = mPan.max = mPan.def = 0;
    mPan.current = -1;
    mTilt.min = mTilt.max = mTilt.def = 0;
    mTilt.current = -1;
    mRoll.min = mRoll.max = mRoll.def = 0;
    mPanRel.min = mPanRel.max = mPanRel.def = 0;
    mPanRel.current = -1;
    mTiltRel.min = mTiltRel.max = mTiltRel.def = 0;
    mTiltRel.current = -1;
    mRollRel.min = mRollRel.max = mRollRel.def = 0;
    mPrivacy.min = mPrivacy.max = mPrivacy.def = 0;
    mPowerlineFrequency.min = mPowerlineFrequency.max = mPowerlineFrequency.def = 0;
    mMultiplier.min = mMultiplier.max = mMultiplier.def = 0;
    mMultiplierLimit.min = mMultiplierLimit.max = mMultiplierLimit.def = 0;
    mAnalogVideoStandard.min = mAnalogVideoStandard.max = mAnalogVideoStandard.def = 0;
    mAnalogVideoLockState.min = mAnalogVideoLockState.max = mAnalogVideoLockState.def = 0;
}


//======================================================================
// Pan(絶対値)調整
int UVCCameraAdjustments::updatePanLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_ACCESS;
//    if (mCtrlSupports & CTRL_PANTILT_ABS) {
//        update_ctrl_values(mDeviceHandle, mPan, mTilt, uvc_get_pantilt_abs);
//    }
    RETURN(ret, int);
}

// Pan(絶対値)を設定
int UVCCameraAdjustments::setPan(int pan) {

    int ret = UVC_ERROR_ACCESS;
//    if (mCtrlSupports & CTRL_PANTILT_ABS) {
//        ret = update_ctrl_values(mDeviceHandle, mPan, mTilt, uvc_get_pantilt_abs);
//        if (LIKELY(!ret)) {
//            pan = pan < mPan.min
//                  ? mPan.min
//                  : (pan > mPan.max ? mPan.max : pan);
//            int tilt = mTilt.current < 0 ? mTilt.def : mTilt.current;
//            ret = uvc_set_pantilt_abs(mDeviceHandle, pan, tilt);
//            if (LIKELY(!ret)) {
//                mPan.current = pan;
//                mTilt.current = tilt;
//            }
//        }
//    }
    RETURN(ret, int);
}

// Pan(絶対値)の現在値を取得
int UVCCameraAdjustments::getPan() {
//    if (mCtrlSupports & CTRL_PANTILT_ABS) {
//        int ret = update_ctrl_values(mDeviceHandle, mPan, mTilt, uvc_get_pantilt_abs);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            int32_t pan, tilt;
//            ret = uvc_get_pantilt_abs(mDeviceHandle, &pan, &tilt, UVC_GET_CUR);
//            if (LIKELY(!ret)) {
//                mPan.current = pan;
//                mTilt.current = tilt;
//                return pan;
//            }
//        }
//    }
    RETURN(0, int);
}

//======================================================================
// Tilt(絶対値)調整
int UVCCameraAdjustments::updateTiltLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_ACCESS;
//    if (mCtrlSupports & CTRL_PANTILT_ABS) {
//        update_ctrl_values(mDeviceHandle, mPan, mTilt, uvc_get_pantilt_abs);
//    }
    RETURN(ret, int);
}

// Tilt(絶対値)を設定
int UVCCameraAdjustments::setTilt(int tilt) {

    int ret = UVC_ERROR_ACCESS;
//    if (mCtrlSupports & CTRL_PANTILT_ABS) {
//        ret = update_ctrl_values(mDeviceHandle, mPan, mTilt, uvc_get_pantilt_abs);
//        if (LIKELY(!ret)) {
//            tilt = tilt < mTilt.min
//                   ? mTilt.min
//                   : (tilt > mTilt.max ? mTilt.max : tilt);
//            int pan = mPan.current < 0 ? mPan.def : mPan.current;
//            ret = uvc_set_pantilt_abs(mDeviceHandle, pan, tilt);
//            if (LIKELY(!ret)) {
//                mPan.current = pan;
//                mTilt.current = tilt;
//            }
//        }
//    }
    RETURN(ret, int);
}

// Tilt(絶対値)の現在値を取得
int UVCCameraAdjustments::getTilt() {

//    if (mCtrlSupports & CTRL_PANTILT_ABS) {
//        int ret = update_ctrl_values(mDeviceHandle, mPan, mTilt, uvc_get_pantilt_abs);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            int32_t pan, tilt;
//            ret = uvc_get_pantilt_abs(mDeviceHandle, &pan, &tilt, UVC_GET_CUR);
//            if (LIKELY(!ret)) {
//                mPan.current = pan;
//                mTilt.current = tilt;
//                return tilt;
//            }
//        }
//    }
    RETURN(0, int);
}

//======================================================================
// Roll(絶対値)調整
int UVCCameraAdjustments::updateRollLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_ACCESS;
//    if (mCtrlSupports & CTRL_ROLL_ABS) {
//        UPDATE_CTRL_VALUES(mRoll, uvc_get_roll_abs);
//    }
    RETURN(ret, int);
}

// Roll(絶対値)を設定
int UVCCameraAdjustments::setRoll(int roll) {

    int ret = UVC_ERROR_ACCESS;
//    if (mCtrlSupports & CTRL_ROLL_ABS) {
//        ret = internalSetCtrlValue(mRoll, roll, uvc_get_roll_abs, uvc_set_roll_abs);
//    }
    RETURN(ret, int);
}

// Roll(絶対値)の現在値を取得
int UVCCameraAdjustments::getRoll() {

//    if (mCtrlSupports & CTRL_ROLL_ABS) {
//        int ret = update_ctrl_values(mDeviceHandle, mRoll, uvc_get_roll_abs);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            int16_t roll;
//            ret = uvc_get_roll_abs(mDeviceHandle, &roll, UVC_GET_CUR);
//            if (LIKELY(!ret)) {
//                mRoll.current = roll;
//                return roll;
//            }
//        }
//    }
    RETURN(0, int);
}


//======================================================================
// スキャニングモード
int UVCCameraAdjustments::updateScanningModeLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & CTRL_SCANNING) {
//        UPDATE_CTRL_VALUES(mScanningMode, uvc_get_scanning_mode);
//    }
    RETURN(ret, int);
}

// スキャニングモードをセット
int UVCCameraAdjustments::setScanningMode(int mode) {

    int r = UVC_ERROR_ACCESS;
//    if LIKELY((mDeviceHandle) && (mCtrlSupports & CTRL_SCANNING)) {
////		LOGI("ae:%d", mode);
//        r = uvc_set_scanning_mode(mDeviceHandle, mode/* & 0xff*/);
//    }
    RETURN(r, int);
}

// スキャニングモード設定を取得
int UVCCameraAdjustments::getScanningMode() {
    int r = UVC_ERROR_ACCESS;
//    if LIKELY((mDeviceHandle) && (mCtrlSupports & CTRL_SCANNING)) {
//        uint8_t mode;
//        r = uvc_get_scanning_mode(mDeviceHandle, &mode, UVC_GET_CUR);
////		LOGI("ae:%d", mode);
//        if (LIKELY(!r)) {
//            r = mode;
//        }
//    }
    RETURN(r, int);
}


//======================================================================
// 露出モード
int UVCCameraAdjustments::updateExposureModeLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & CTRL_AE) {
//        UPDATE_CTRL_VALUES(mExposureMode, uvc_get_ae_mode);
//    }
    RETURN(ret, int);
}

// 露出をセット
int UVCCameraAdjustments::setExposureMode(int mode) {

    int r = UVC_ERROR_ACCESS;
//    if LIKELY((mDeviceHandle) && (mCtrlSupports & CTRL_AE)) {
////		LOGI("ae:%d", mode);
//        r = uvc_set_ae_mode(mDeviceHandle, mode/* & 0xff*/);
//    }
    RETURN(r, int);
}

// 露出設定を取得
int UVCCameraAdjustments::getExposureMode() {
    int r = UVC_ERROR_ACCESS;
//    if LIKELY((mDeviceHandle) && (mCtrlSupports & CTRL_AE)) {
//        uint8_t mode;
//        r = uvc_get_ae_mode(mDeviceHandle, &mode, UVC_GET_CUR);
////		LOGI("ae:%d", mode);
//        if (LIKELY(!r)) {
//            r = mode;
//        }
//    }
    RETURN(r, int);
}

//======================================================================
// 露出優先設定
int UVCCameraAdjustments::updateExposurePriorityLimit(int &min, int &max, int &def) {
    int ret = UVC_ERROR_IO;
//    if (mPUSupports & CTRL_AE_PRIORITY) {
//        UPDATE_CTRL_VALUES(mExposurePriority, uvc_get_ae_priority);
//    }
    RETURN(ret, int);
}

// 露出優先設定をセット
int UVCCameraAdjustments::setExposurePriority(int priority) {

    int r = UVC_ERROR_ACCESS;
//    if LIKELY((mDeviceHandle) && (mCtrlSupports & CTRL_AE_PRIORITY)) {
////		LOGI("ae priority:%d", priority);
//        r = uvc_set_ae_priority(mDeviceHandle, priority/* & 0xff*/);
//    }
    RETURN(r, int);
}

// 露出優先設定を取得
int UVCCameraAdjustments::getExposurePriority() {
    int r = UVC_ERROR_ACCESS;
//    if LIKELY((mDeviceHandle) && (mCtrlSupports & CTRL_AE_PRIORITY)) {
//        uint8_t priority;
//        r = uvc_get_ae_priority(mDeviceHandle, &priority, UVC_GET_CUR);
////		LOGI("ae priority:%d", priority);
//        if (LIKELY(!r)) {
//            r = priority;
//        }
//    }
    RETURN(r, int);
}

//======================================================================
// 露出(絶対値)設定
int UVCCameraAdjustments::updateExposureLimit(int &min, int &max, int &def) {
    int ret = UVC_ERROR_IO;
//    if (mPUSupports & CTRL_AE_ABS) {
//        UPDATE_CTRL_VALUES(mExposureAbs, uvc_get_exposure_abs);
//    }
    RETURN(ret, int);
}

// 露出(絶対値)設定をセット
int UVCCameraAdjustments::setExposure(int ae_abs) {

    int r = UVC_ERROR_ACCESS;
//    if LIKELY((mDeviceHandle) && (mCtrlSupports & CTRL_AE_ABS)) {
////		LOGI("ae_abs:%d", ae_abs);
//        r = uvc_set_exposure_abs(mDeviceHandle, ae_abs/* & 0xff*/);
//    }
    RETURN(r, int);
}

// 露出(絶対値)設定を取得
int UVCCameraAdjustments::getExposure() {
    int r = UVC_ERROR_ACCESS;
//    if LIKELY((mDeviceHandle) && (mCtrlSupports & CTRL_AE_ABS)) {
//        uint32_t ae_abs;
//        r = uvc_get_exposure_abs(mDeviceHandle, &ae_abs, UVC_GET_CUR);
////		LOGI("ae_abs:%d", ae_abs);
//        if (LIKELY(!r)) {
//            r = ae_abs;
//        }
//    }
    RETURN(r, int);
}

//======================================================================
// 露出(相対値)設定
int UVCCameraAdjustments::updateExposureRelLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & CTRL_AE_REL) {
//        UPDATE_CTRL_VALUES(mExposureAbs, uvc_get_exposure_rel);
//    }
    RETURN(ret, int);
}

// 露出(相対値)設定をセット
int UVCCameraAdjustments::setExposureRel(int ae_rel) {
    int r = UVC_ERROR_ACCESS;
//    if LIKELY((mDeviceHandle) && (mCtrlSupports & CTRL_AE_REL)) {
////		LOGI("ae_rel:%d", ae_rel);
//        r = uvc_set_exposure_rel(mDeviceHandle, ae_rel/* & 0xff*/);
//    }
    RETURN(r, int);
}

// 露出(相対値)設定を取得
int UVCCameraAdjustments::getExposureRel() {
    int r = UVC_ERROR_ACCESS;
//    if LIKELY((mDeviceHandle) && (mCtrlSupports & CTRL_AE_REL)) {
//        int8_t ae_rel;
//        r = uvc_get_exposure_rel(mDeviceHandle, &ae_rel, UVC_GET_CUR);
////		LOGI("ae_rel:%d", ae_rel);
//        if (LIKELY(!r)) {
//            r = ae_rel;
//        }
//    }
    RETURN(r, int);
}

//======================================================================
// オートフォーカス
int UVCCameraAdjustments::updateAutoFocusLimit(int &min, int &max, int &def) {
    int ret = UVC_ERROR_IO;
//    if (mPUSupports & CTRL_FOCUS_AUTO) {
//        UPDATE_CTRL_VALUES(mAutoFocus, uvc_get_focus_auto);
//    }
    RETURN(ret, int);
}

// オートフォーカスをon/off
int UVCCameraAdjustments::setAutoFocus(bool autoFocus) {
    int r = UVC_ERROR_ACCESS;
//    if LIKELY((mDeviceHandle) && (mCtrlSupports & CTRL_FOCUS_AUTO)) {
//        r = uvc_set_focus_auto(mDeviceHandle, autoFocus);
//    }
    RETURN(r, int);
}

// オートフォーカスのon/off状態を取得
bool UVCCameraAdjustments::getAutoFocus() {
    int r = UVC_ERROR_ACCESS;
//    if LIKELY((mDeviceHandle) && (mCtrlSupports & CTRL_FOCUS_AUTO)) {
//        uint8_t autoFocus;
//        r = uvc_get_focus_auto(mDeviceHandle, &autoFocus, UVC_GET_CUR);
//        if (LIKELY(!r))
//            r = autoFocus;
//    }
    RETURN(r, int);
}

//======================================================================
// フォーカス(絶対値)調整
int UVCCameraAdjustments::updateFocusLimit(int &min, int &max, int &def) {
    int ret = UVC_ERROR_ACCESS;
//    if (mCtrlSupports & CTRL_FOCUS_ABS) {
//        UPDATE_CTRL_VALUES(mFocus, uvc_get_focus_abs);
//    }
    RETURN(ret, int);
}

// フォーカス(絶対値)を設定
int UVCCameraAdjustments::setFocus(int focus) {
    int ret = UVC_ERROR_ACCESS;
//    if (mCtrlSupports & CTRL_FOCUS_ABS) {
//        ret = internalSetCtrlValue(mFocus, focus, uvc_get_focus_abs, uvc_set_focus_abs);
//    }
    RETURN(ret, int);
}

// フォーカス(絶対値)の現在値を取得
int UVCCameraAdjustments::getFocus() {
//    if (mCtrlSupports & CTRL_FOCUS_ABS) {
//        int ret = update_ctrl_values(mDeviceHandle, mFocus, uvc_get_focus_abs);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            uint16_t value;
//            ret = uvc_get_focus_abs(mDeviceHandle, &value, UVC_GET_CUR);
//            if (LIKELY(!ret))
//                return value;
//        }
//    }
    RETURN(0, int);
}


//======================================================================
// フォーカス(相対値)調整
int UVCCameraAdjustments::updateFocusRelLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_ACCESS;
//    if (mCtrlSupports & CTRL_FOCUS_REL) {
//        UPDATE_CTRL_VALUES(mFocusRel, uvc_get_focus_rel);
//    }
    RETURN(ret, int);
}

// フォーカス(相対値)を設定
int UVCCameraAdjustments::setFocusRel(int focus_rel) {

    int ret = UVC_ERROR_ACCESS;
//    if (mCtrlSupports & CTRL_FOCUS_REL) {
//        ret = internalSetCtrlValue(mFocusRel, (int8_t) ((focus_rel >> 8) & 0xff),
//                                   (uint8_t) (focus_rel & 0xff),
//                                   uvc_get_focus_rel, uvc_set_focus_rel);
//    }
    RETURN(ret, int);
}

// フォーカス(相対値)の現在値を取得
int UVCCameraAdjustments::getFocusRel() {

//    if (mCtrlSupports & CTRL_FOCUS_REL) {
//        int ret = update_ctrl_values(mDeviceHandle, mFocusRel, uvc_get_focus_abs);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            int8_t focus;
//            uint8_t speed;
//            ret = uvc_get_focus_rel(mDeviceHandle, &focus, &speed, UVC_GET_CUR);
//            if (LIKELY(!ret))
//                return (focus << 8) + speed;
//        }
//    }
    RETURN(0, int);
}

UVCCameraAdjustments::UVCCameraAdjustments(uvc_device_handle_t *deviceHandle)
        : mDeviceHandle(deviceHandle) {

}

//======================================================================
/*
// フォーカス(シンプル)調整
int UVCCameraConfig::updateFocusSimpleLimit(int &min, int &max, int &def) {
	
	int ret = UVC_ERROR_ACCESS;
	if (mCtrlSupports & CTRL_FOCUS_SIMPLE) {
		UPDATE_CTRL_VALUES(mFocusSimple, uvc_get_focus_simple_range);
	}
	RETURN(ret, int);
}

// フォーカス(シンプル)を設定
int UVCCameraConfig::setFocusSimple(int focus) {
	
	int ret = UVC_ERROR_ACCESS;
	if (mCtrlSupports & CTRL_FOCUS_SIMPLE) {
		ret = internalSetCtrlValue(mFocusSimple, focus, uvc_get_focus_simple_range, uvc_set_focus_simple_range);
	}
	RETURN(ret, int);
}

// フォーカス(シンプル)の現在値を取得
int UVCCameraConfig::getFocusSimple() {
	
	if (mCtrlSupports & CTRL_FOCUS_SIMPLE) {
		int ret = update_ctrl_values(mDeviceHandle, mFocusSimple, uvc_get_focus_abs);
		if (LIKELY(!ret)) {	// 正常に最小・最大値を取得出来た時
			uint8_t value;
			ret = uvc_get_focus_simple_range(mDeviceHandle, &value, UVC_GET_CUR);
			if (LIKELY(!ret))
				return value;
		}
	}
	RETURN(0, int);
}
*/

//======================================================================
// 絞り(絶対値)調整
int UVCCameraAdjustments::updateIrisLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_ACCESS;
//    if (mCtrlSupports & CTRL_IRIS_ABS) {
//        UPDATE_CTRL_VALUES(mIris, uvc_get_iris_abs);
//    }
    RETURN(ret, int);
}

// 絞り(絶対値)を設定
int UVCCameraAdjustments::setIris(int iris) {

    int ret = UVC_ERROR_ACCESS;
//    if (mCtrlSupports & CTRL_IRIS_ABS) {
//        ret = internalSetCtrlValue(mIris, iris, uvc_get_iris_abs, uvc_set_iris_abs);
//    }
    RETURN(ret, int);
}

// 絞り(絶対値)の現在値を取得
int UVCCameraAdjustments::getIris() {

//    if (mCtrlSupports & CTRL_IRIS_ABS) {
//        int ret = update_ctrl_values(mDeviceHandle, mIris, uvc_get_iris_abs);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            uint16_t value;
//            ret = uvc_get_iris_abs(mDeviceHandle, &value, UVC_GET_CUR);
//            if (LIKELY(!ret))
//                return value;
//        }
//    }
    RETURN(0, int);
}

//======================================================================
// 絞り(相対値)調整
int UVCCameraAdjustments::updateIrisRelLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_ACCESS;
//    if (mCtrlSupports & CTRL_IRIS_REL) {
//        UPDATE_CTRL_VALUES(mIris, uvc_get_iris_rel);
//    }
    RETURN(ret, int);
}

// 絞り(相対値)を設定
int UVCCameraAdjustments::setIrisRel(int iris_rel) {

    int ret = UVC_ERROR_ACCESS;
//    if (mCtrlSupports & CTRL_IRIS_REL) {
//        ret = internalSetCtrlValue(mIris, iris_rel, uvc_get_iris_rel, uvc_set_iris_rel);
//    }
    RETURN(ret, int);
}

// 絞り(相対値)の現在値を取得
int UVCCameraAdjustments::getIrisRel() {

//    if (mCtrlSupports & CTRL_IRIS_REL) {
//        int ret = update_ctrl_values(mDeviceHandle, mIris, uvc_get_iris_rel);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            uint8_t iris_rel;
//            ret = uvc_get_iris_rel(mDeviceHandle, &iris_rel, UVC_GET_CUR);
//            if (LIKELY(!ret))
//                return iris_rel;
//        }
//    }
    RETURN(0, int);
}

//======================================================================
int UVCCameraAdjustments::updatePanRelLimit(int &min, int &max, int &def) {

    // FIXME not implemented yet
    RETURN(UVC_ERROR_ACCESS, int);
}

int UVCCameraAdjustments::setPanRel(int pan_rel) {

    // FIXME not implemented yet
    RETURN(UVC_ERROR_ACCESS, int);
}

int UVCCameraAdjustments::getPanRel() {

    // FIXME not implemented yet
    RETURN(UVC_ERROR_ACCESS, int);
}

//======================================================================
int UVCCameraAdjustments::updateTiltRelLimit(int &min, int &max, int &def) {

    // FIXME not implemented yet
    RETURN(UVC_ERROR_ACCESS, int);
}

int UVCCameraAdjustments::setTiltRel(int tilt_rel) {

    // FIXME not implemented yet
    RETURN(UVC_ERROR_ACCESS, int);
}

int UVCCameraAdjustments::getTiltRel() {

    // FIXME not implemented yet
    RETURN(UVC_ERROR_ACCESS, int);
}

//======================================================================
int UVCCameraAdjustments::updateRollRelLimit(int &min, int &max, int &def) {

    // FIXME not implemented yet
    RETURN(UVC_ERROR_ACCESS, int);
}

int UVCCameraAdjustments::setRollRel(int roll_rel) {

    // FIXME not implemented yet
    RETURN(UVC_ERROR_ACCESS, int);
}

int UVCCameraAdjustments::getRollRel() {

    // FIXME not implemented yet
    RETURN(UVC_ERROR_ACCESS, int);
}

//======================================================================
// プライバシーモード
int UVCCameraAdjustments::updatePrivacyLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_ACCESS;
//    if (mCtrlSupports & CTRL_PRIVACY) {
//        UPDATE_CTRL_VALUES(mPrivacy, uvc_get_focus_abs);
//    }
    RETURN(ret, int);
}

// プライバシーモードを設定
int UVCCameraAdjustments::setPrivacy(int privacy) {

    int ret = UVC_ERROR_ACCESS;
//    if (mCtrlSupports & CTRL_PRIVACY) {
//        ret = internalSetCtrlValue(mPrivacy, privacy, uvc_get_privacy, uvc_set_privacy);
//    }
    RETURN(ret, int);
}

// プライバシーモードの現在値を取得
int UVCCameraAdjustments::getPrivacy() {

//    if (mCtrlSupports & CTRL_PRIVACY) {
//        int ret = update_ctrl_values(mDeviceHandle, mPrivacy, uvc_get_privacy);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            uint8_t privacy;
//            ret = uvc_get_privacy(mDeviceHandle, &privacy, UVC_GET_CUR);
//            if (LIKELY(!ret))
//                return privacy;
//        }
//    }
    RETURN(0, int);
}

//======================================================================
/*
// DigitalWindow
int UVCCameraConfig::updateDigitalWindowLimit(...not defined...) {
	
	// FIXME not implemented yet
	RETURN(UVC_ERROR_ACCESS, int);
}

// DigitalWindowを設定
int UVCCameraConfig::setDigitalWindow(int top, int reft, int bottom, int right) {
	
	// FIXME not implemented yet
	RETURN(UVC_ERROR_ACCESS, int);
}

// DigitalWindowの現在値を取得
int UVCCameraConfig::getDigitalWindow(int &top, int &reft, int &bottom, int &right) {
	
	// FIXME not implemented yet
	RETURN(UVC_ERROR_ACCESS, int);
}
*/

//======================================================================
/*
// DigitalRoi
int UVCCameraConfig::updateDigitalRoiLimit(...not defined...) {
	
	// FIXME not implemented yet
	RETURN(UVC_ERROR_ACCESS, int);
}

// DigitalRoiを設定
int UVCCameraConfig::setDigitalRoi(int top, int reft, int bottom, int right) {
	
	// FIXME not implemented yet
	RETURN(UVC_ERROR_ACCESS, int);
}

// DigitalRoiの現在値を取得
int UVCCameraConfig::getDigitalRoi(int &top, int &reft, int &bottom, int &right) {
	
	// FIXME not implemented yet
	RETURN(UVC_ERROR_ACCESS, int);
}
*/

//======================================================================
// backlight_compensation
int UVCCameraAdjustments::updateBacklightCompLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_BACKLIGHT) {
//        UPDATE_CTRL_VALUES(mBacklightComp, uvc_get_backlight_compensation);
//    }
    RETURN(ret, int);
}

// backlight_compensationを設定
int UVCCameraAdjustments::setBacklightComp(int backlight) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_BACKLIGHT) {
//        ret = internalSetCtrlValue(mBacklightComp, backlight, uvc_get_backlight_compensation,
//                                   uvc_set_backlight_compensation);
//    }
    RETURN(ret, int);
}

// backlight_compensationの現在値を取得
int UVCCameraAdjustments::getBacklightComp() {

//    if (mPUSupports & PU_BACKLIGHT) {
//        int ret = update_ctrl_values(mDeviceHandle, mBacklightComp, uvc_get_backlight_compensation);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            uint16_t value;
//            ret = uvc_get_backlight_compensation(mDeviceHandle, &value, UVC_GET_CUR);
//            if (LIKELY(!ret))
//                return value;
//        }
//    }
    RETURN(0, int);
}


//======================================================================
// 明るさ
int UVCCameraAdjustments::updateBrightnessLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_BRIGHTNESS) {
//        UPDATE_CTRL_VALUES(mBrightness, uvc_get_brightness);
//    }
    RETURN(ret, int);
}

int UVCCameraAdjustments::setBrightness(int brightness) {
    LOGE("setBrightness 1  %d", brightness);
    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_BRIGHTNESS) {
//        ret = internalSetCtrlValue(mBrightness, brightness, uvc_get_brightness, uvc_set_brightness);
//    }
    LOGE("setBrightness %d = %d", brightness, ret);
    return ret;
}

// 明るさの現在値を取得
int UVCCameraAdjustments::getBrightness() {

//    if (mPUSupports & PU_BRIGHTNESS) {
//        int ret = update_ctrl_values(mDeviceHandle, mBrightness, uvc_get_brightness);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            int16_t value;
//            ret = uvc_get_brightness(mDeviceHandle, &value, UVC_GET_CUR);
//            if (LIKELY(!ret))
//                return value;
//        }
//    }
    RETURN(0, int);
}

//======================================================================
// コントラスト調整
int UVCCameraAdjustments::updateContrastLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_CONTRAST) {
//        UPDATE_CTRL_VALUES(mContrast, uvc_get_contrast);
//    }
    RETURN(ret, int);
}

// コントラストを設定
int UVCCameraAdjustments::setContrast(uint16_t contrast) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_CONTRAST) {
//        ret = internalSetCtrlValue(mContrast, contrast, uvc_get_contrast, uvc_set_contrast);
//    }
    RETURN(ret, int);
}

// コントラストの現在値を取得
int UVCCameraAdjustments::getContrast() {

//    if (mPUSupports & PU_CONTRAST) {
//        int ret = update_ctrl_values(mDeviceHandle, mContrast, uvc_get_contrast);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            uint16_t value;
//            ret = uvc_get_contrast(mDeviceHandle, &value, UVC_GET_CUR);
//            if (LIKELY(!ret))
//                return value;
//        }
//    }
    RETURN(0, int);
}

//======================================================================
// オートコントラスト
int UVCCameraAdjustments::updateAutoContrastLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_CONTRAST_AUTO) {
//        UPDATE_CTRL_VALUES(mAutoFocus, uvc_get_contrast_auto);
//    }
    RETURN(ret, int);
}

// オートコントラストをon/off
int UVCCameraAdjustments::setAutoContrast(bool autoContrast) {
    int r = UVC_ERROR_ACCESS;
//    if LIKELY((mDeviceHandle) && (mPUSupports & PU_CONTRAST_AUTO)) {
//        r = uvc_set_contrast_auto(mDeviceHandle, autoContrast);
//    }
    RETURN(r, int);
}

// オートコントラストのon/off状態を取得
bool UVCCameraAdjustments::getAutoContrast() {
    int r = UVC_ERROR_ACCESS;
//    if LIKELY((mDeviceHandle) && (mPUSupports & PU_CONTRAST_AUTO)) {
//        uint8_t autoContrast;
//        r = uvc_get_contrast_auto(mDeviceHandle, &autoContrast, UVC_GET_CUR);
//        if (LIKELY(!r))
//            r = autoContrast;
//    }
    RETURN(r, int);
}

//======================================================================
// シャープネス調整
int UVCCameraAdjustments::updateSharpnessLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_SHARPNESS) {
//        UPDATE_CTRL_VALUES(mSharpness, uvc_get_sharpness);
//    }
    RETURN(ret, int);
}

// シャープネスを設定
int UVCCameraAdjustments::setSharpness(int sharpness) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_SHARPNESS) {
//        ret = internalSetCtrlValue(mSharpness, sharpness, uvc_get_sharpness, uvc_set_sharpness);
//    }
    RETURN(ret, int);
}

// シャープネスの現在値を取得
int UVCCameraAdjustments::getSharpness() {

//    if (mPUSupports & PU_SHARPNESS) {
//        int ret = update_ctrl_values(mDeviceHandle, mSharpness, uvc_get_sharpness);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            uint16_t value;
//            ret = uvc_get_sharpness(mDeviceHandle, &value, UVC_GET_CUR);
//            if (LIKELY(!ret))
//                return value;
//        }
//    }
    RETURN(0, int);
}

//======================================================================
// ゲイン調整
int UVCCameraAdjustments::updateGainLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
    if (mPUSupports & PU_GAIN) {
//        UPDATE_CTRL_VALUES(mGain, uvc_get_gain)
    }
    RETURN(ret, int);
}

// ゲインを設定
int UVCCameraAdjustments::setGain(int gain) {

    int ret = UVC_ERROR_IO;
    if (mPUSupports & PU_GAIN) {
//		LOGI("gain:%d", gain);
//        ret = internalSetCtrlValue(mGain, gain, uvc_get_gain, uvc_set_gain);
    }
    RETURN(ret, int);
}

// ゲインの現在値を取得
int UVCCameraAdjustments::getGain() {

//    if (mPUSupports & PU_GAIN) {
//        int ret = update_ctrl_values(mDeviceHandle, mGain, uvc_get_gain);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            uint16_t value;
////            ret = uvc_get_gain(mDeviceHandle, &value, UVC_GET_CUR);
////			LOGI("gain:%d", value);
//            if (LIKELY(!ret))
//                return value;
//        }
//    }
    RETURN(0, int);
}


//======================================================================
// オートホワイトバランス(temp)
int UVCCameraAdjustments::updateAutoWhiteBlanceLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
    if (mPUSupports & PU_WB_TEMP_AUTO) {
//        UPDATE_CTRL_VALUES(mAutoWhiteBlance, uvc_get_white_balance_temperature_auto);
    }
    RETURN(ret, int);
}

// オートホワイトバランス(temp)をon/off
int UVCCameraAdjustments::setAutoWhiteBlance(bool autoWhiteBlance) {

    int r = UVC_ERROR_ACCESS;
    if LIKELY((mDeviceHandle) && (mPUSupports & PU_WB_TEMP_AUTO)) {
//        r = uvc_set_white_balance_temperature_auto(mDeviceHandle, autoWhiteBlance);
    }
    RETURN(r, int);
}

// オートホワイトバランス(temp)のon/off状態を取得
bool UVCCameraAdjustments::getAutoWhiteBlance() {

    int r = UVC_ERROR_ACCESS;
    if LIKELY((mDeviceHandle) && (mPUSupports & PU_WB_TEMP_AUTO)) {
        uint8_t autoWhiteBlance;
//        r = uvc_get_white_balance_temperature_auto(mDeviceHandle, &autoWhiteBlance, UVC_GET_CUR);
        if (LIKELY(!r))
            r = autoWhiteBlance;
    }
    RETURN(r, int);
}

//======================================================================
// オートホワイトバランス(compo)
int UVCCameraAdjustments::updateAutoWhiteBlanceCompoLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
    if (mPUSupports & PU_WB_COMPO_AUTO) {
//        UPDATE_CTRL_VALUES(mAutoWhiteBlanceCompo, uvc_get_white_balance_component_auto);
    }
    RETURN(ret, int);
}

// オートホワイトバランス(compo)をon/off
int UVCCameraAdjustments::setAutoWhiteBlanceCompo(bool autoWhiteBlanceCompo) {

    int r = UVC_ERROR_ACCESS;
    if LIKELY((mDeviceHandle) && (mPUSupports & PU_WB_COMPO_AUTO)) {
//        r = uvc_set_white_balance_component_auto(mDeviceHandle, autoWhiteBlanceCompo);
    }
    RETURN(r, int);
}

// オートホワイトバランス(compo)のon/off状態を取得
bool UVCCameraAdjustments::getAutoWhiteBlanceCompo() {

    int r = UVC_ERROR_ACCESS;
    if LIKELY((mDeviceHandle) && (mPUSupports & PU_WB_COMPO_AUTO)) {
        uint8_t autoWhiteBlanceCompo;
//        r = uvc_get_white_balance_component_auto(mDeviceHandle, &autoWhiteBlanceCompo, UVC_GET_CUR);
        if (LIKELY(!r))
            r = autoWhiteBlanceCompo;
    }
    RETURN(r, int);
}

//======================================================================
// ホワイトバランス色温度調整
int UVCCameraAdjustments::updateWhiteBlanceLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
    if (mPUSupports & PU_WB_TEMP) {
//        UPDATE_CTRL_VALUES(mWhiteBlance, uvc_get_white_balance_temperature)
    }
    RETURN(ret, int);
}

// ホワイトバランス色温度を設定
int UVCCameraAdjustments::setWhiteBlance(int white_blance) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_WB_TEMP) {
//        ret = internalSetCtrlValue(mWhiteBlance, white_blance,
//                                   uvc_get_white_balance_temperature, uvc_set_white_balance_temperature);
//    }
    RETURN(ret, int);
}

// ホワイトバランス色温度の現在値を取得
int UVCCameraAdjustments::getWhiteBlance() {

//    if (mPUSupports & PU_WB_TEMP) {
//        int ret = update_ctrl_values(mDeviceHandle, mWhiteBlance, uvc_get_white_balance_temperature);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            uint16_t value;
//            ret = uvc_get_white_balance_temperature(mDeviceHandle, &value, UVC_GET_CUR);
//            if (LIKELY(!ret))
//                return value;
//        }
//    }
    RETURN(0, int);
}

//======================================================================
// ホワイトバランスcompo調整
int UVCCameraAdjustments::updateWhiteBlanceCompoLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
    if (mPUSupports & PU_WB_COMPO) {
        // TODO not implemented
        //UPDATE_CTRL_VALUES(mWhiteBlanceCompo, uvc_get_white_balance_component)
    }
    RETURN(ret, int);
}

// ホワイトバランスcompoを設定
int UVCCameraAdjustments::setWhiteBlanceCompo(int white_blance_compo) {

    int ret = UVC_ERROR_IO;
    if (mPUSupports & PU_WB_COMPO) {
        // TODO not implemented
//        ret = internalSetCtrlValue(mWhiteBlanceCompo, white_blance_compo,
//                                   uvc_get_white_balance_component, uvc_set_white_balance_component);
    }
    RETURN(ret, int);
}

// ホワイトバランスcompoの現在値を取得
int UVCCameraAdjustments::getWhiteBlanceCompo() {

    if (mPUSupports & PU_WB_COMPO) {
        // TODO not implemented
//        int ret = update_ctrl_values(mDeviceHandle, mWhiteBlanceCompo, uvc_get_white_balance_component);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            uint16_t blue;
//            uint16_t red;
//            ret = uvc_get_white_balance_component(mDeviceHandle, &blue, &red, UVC_GET_CUR);
//            if (LIKELY(!ret))
//                return 0;
//        }
    }
    RETURN(0, int);
}

//======================================================================
// ガンマ調整
int UVCCameraAdjustments::updateGammaLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
    if (mPUSupports & PU_GAMMA) {
//        UPDATE_CTRL_VALUES(mGamma, uvc_get_gamma)
    }
    RETURN(ret, int);
}

// ガンマを設定
int UVCCameraAdjustments::setGamma(int gamma) {

    int ret = UVC_ERROR_IO;
    if (mPUSupports & PU_GAMMA) {
//		LOGI("gamma:%d", gamma);
//        ret = internalSetCtrlValue(mGamma, gamma, uvc_get_gamma, uvc_set_gamma);
    }
    RETURN(ret, int);
}

// ガンマの現在値を取得
int UVCCameraAdjustments::getGamma() {

//    if (mPUSupports & PU_GAMMA) {
//        int ret = update_ctrl_values(mDeviceHandle, mGamma, uvc_get_gamma);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            uint16_t value;
//            ret = uvc_get_gamma(mDeviceHandle, &value, UVC_GET_CUR);
////			LOGI("gamma:%d", ret);
//            if (LIKELY(!ret))
//                return value;
//        }
//    }
    RETURN(0, int);
}

//======================================================================
// 彩度調整
int UVCCameraAdjustments::updateSaturationLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_SATURATION) {
//        UPDATE_CTRL_VALUES(mSaturation, uvc_get_saturation)
//    }
    RETURN(ret, int);
}

// 彩度を設定
int UVCCameraAdjustments::setSaturation(int saturation) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_SATURATION) {
//        ret = internalSetCtrlValue(mSaturation, saturation, uvc_get_saturation, uvc_set_saturation);
//    }
    RETURN(ret, int);
}

// 彩度の現在値を取得
int UVCCameraAdjustments::getSaturation() {

//    if (mPUSupports & PU_SATURATION) {
//        int ret = update_ctrl_values(mDeviceHandle, mSaturation, uvc_get_saturation);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            uint16_t value;
//            ret = uvc_get_saturation(mDeviceHandle, &value, UVC_GET_CUR);
//            if (LIKELY(!ret))
//                return value;
//        }
//    }
    RETURN(0, int);
}


//======================================================================
// 色相調整
int UVCCameraAdjustments::updateHueLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_HUE) {
//        UPDATE_CTRL_VALUES(mHue, uvc_get_hue)
//    }
    RETURN(ret, int);
}

// 色相を設定
int UVCCameraAdjustments::setHue(int hue) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_HUE) {
//        ret = internalSetCtrlValue(mHue, hue, uvc_get_hue, uvc_set_hue);
//    }
    RETURN(ret, int);
}

// 色相の現在値を取得
int UVCCameraAdjustments::getHue() {

//    if (mPUSupports & PU_HUE) {
//        int ret = update_ctrl_values(mDeviceHandle, mHue, uvc_get_hue);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            int16_t value;
//            ret = uvc_get_hue(mDeviceHandle, &value, UVC_GET_CUR);
//            if (LIKELY(!ret))
//                return value;
//        }
//    }
    RETURN(0, int);
}

//======================================================================
// オート色相
int UVCCameraAdjustments::updateAutoHueLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_HUE_AUTO) {
//        UPDATE_CTRL_VALUES(mAutoHue, uvc_get_hue_auto);
//    }
    RETURN(ret, int);
}

// オート色相をon/off
int UVCCameraAdjustments::setAutoHue(bool autoHue) {


    int r = UVC_ERROR_ACCESS;
    if LIKELY((mDeviceHandle) && (mPUSupports & PU_HUE_AUTO)) {
//        r = uvc_set_hue_auto(mDeviceHandle, autoHue);
    }
    RETURN(r, int);
}

// オート色相のon/off状態を取得
bool UVCCameraAdjustments::getAutoHue() {

    int r = UVC_ERROR_ACCESS;
//    if LIKELY((mDeviceHandle) && (mPUSupports & PU_HUE_AUTO)) {
//        uint8_t autoHue;
//        r = uvc_get_hue_auto(mDeviceHandle, &autoHue, UVC_GET_CUR);
//        if (LIKELY(!r))
//            r = autoHue;
//    }
    RETURN(r, int);
}

//======================================================================
// 電源周波数によるチラつき補正
int UVCCameraAdjustments::updatePowerlineFrequencyLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
//    if (mCtrlSupports & PU_POWER_LF) {
//        UPDATE_CTRL_VALUES(mPowerlineFrequency, uvc_get_power_line_frequency)
//    }
    RETURN(ret, int);
}

// 電源周波数によるチラつき補正を設定
int UVCCameraAdjustments::setPowerlineFrequency(int frequency) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_POWER_LF) {
//        if (frequency < 0) {
//            uint8_t value;
//            ret = uvc_get_power_line_frequency(mDeviceHandle, &value, UVC_GET_DEF);
//            if LIKELY(ret)
//                frequency = value;
//            else RETURN(ret, int);
//        }
//        LOGD("frequency:%d", frequency);
//        ret = uvc_set_power_line_frequency(mDeviceHandle, frequency);
//    }

    RETURN(ret, int);
}

// 電源周波数によるチラつき補正値を取得
int UVCCameraAdjustments::getPowerlineFrequency() {

    if (mPUSupports & PU_POWER_LF) {
        uint8_t value;
//        int ret = uvc_get_power_line_frequency(mDeviceHandle, &value, UVC_GET_CUR);
//        LOGD("frequency:%d", ret);
//        if (LIKELY(!ret))
//            return value;
    }
    RETURN(0, int);
}

//======================================================================
// ズーム(abs)調整
int UVCCameraAdjustments::updateZoomLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
    if (mCtrlSupports & CTRL_ZOOM_ABS) {
//        UPDATE_CTRL_VALUES(mZoom, uvc_get_zoom_abs)
    }
    RETURN(ret, int);
}

// ズーム(abs)を設定
int UVCCameraAdjustments::setZoom(int zoom) {

    int ret = UVC_ERROR_IO;
//    if (mCtrlSupports & CTRL_ZOOM_ABS) {
//        ret = internalSetCtrlValue(mZoom, zoom, uvc_get_zoom_abs, uvc_set_zoom_abs);
//    }
    RETURN(ret, int);
}

// ズーム(abs)の現在値を取得
int UVCCameraAdjustments::getZoom() {

//    if (mCtrlSupports & CTRL_ZOOM_ABS) {
//        int ret = update_ctrl_values(mDeviceHandle, mZoom, uvc_get_zoom_abs);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            uint16_t value;
//            ret = uvc_get_zoom_abs(mDeviceHandle, &value, UVC_GET_CUR);
//            if (LIKELY(!ret))
//                return value;
//        }
//    }
    RETURN(0, int);
}

//======================================================================
// Control UVC Camera
int UVCCameraAdjustments::sendCommand(int command) {

    int ret = UVC_ERROR_IO;
//    ret = internalSetCtrlValue(command, uvc_set_zoom_abs);
    RETURN(ret, int);
}

//======================================================================
// ズーム(相対値)調整
int UVCCameraAdjustments::updateZoomRelLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
    if (mCtrlSupports & CTRL_ZOOM_REL) {
//        UPDATE_CTRL_VALUES(mZoomRel, uvc_get_zoom_rel)
    }
    RETURN(ret, int);
}

// ズーム(相対値)を設定
int UVCCameraAdjustments::setZoomRel(int zoom) {

    int ret = UVC_ERROR_IO;
//    if (mCtrlSupports & CTRL_ZOOM_REL) {
//        ret = internalSetCtrlValue(mZoomRel,
//                                   (int8_t) ((zoom >> 16) & 0xff), (uint8_t) ((zoom >> 8) & 0xff),
//                                   (uint8_t) (zoom & 0xff),
//                                   uvc_get_zoom_rel, uvc_set_zoom_rel);
//    }
    RETURN(ret, int);
}

// ズーム(相対値)の現在値を取得
int UVCCameraAdjustments::getZoomRel() {

//    if (mCtrlSupports & CTRL_ZOOM_REL) {
//        int ret = update_ctrl_values(mDeviceHandle, mZoomRel, uvc_get_zoom_rel);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            int8_t zoom;
//            uint8_t isdigital;
//            uint8_t speed;
//            ret = uvc_get_zoom_rel(mDeviceHandle, &zoom, &isdigital, &speed, UVC_GET_CUR);
//            if (LIKELY(!ret))
//                return (zoom << 16) + (isdigital << 8) + speed;
//        }
//    }
    RETURN(0, int);
}

//======================================================================
// digital multiplier調整
int UVCCameraAdjustments::updateDigitalMultiplierLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_DIGITAL_MULT) {
//        UPDATE_CTRL_VALUES(mMultiplier, uvc_get_digital_multiplier)
//    }
    RETURN(ret, int);
}

// digital multiplierを設定
int UVCCameraAdjustments::setDigitalMultiplier(int multiplier) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_DIGITAL_MULT) {
////		LOGI("multiplier:%d", multiplier);
//        ret = internalSetCtrlValue(mMultiplier, multiplier, uvc_get_digital_multiplier, uvc_set_digital_multiplier);
//    }
    RETURN(ret, int);
}

// digital multiplierの現在値を取得
int UVCCameraAdjustments::getDigitalMultiplier() {

//    if (mPUSupports & PU_DIGITAL_MULT) {
//        int ret = update_ctrl_values(mDeviceHandle, mMultiplier, uvc_get_digital_multiplier);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            uint16_t multiplier;
//            ret = uvc_get_digital_multiplier(mDeviceHandle, &multiplier, UVC_GET_CUR);
////			LOGI("multiplier:%d", multiplier);
//            if (LIKELY(!ret))
//                return multiplier;
//        }
//    }
    RETURN(0, int);
}

//======================================================================
// digital multiplier limit調整
int UVCCameraAdjustments::updateDigitalMultiplierLimitLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_DIGITAL_LIMIT) {
//        UPDATE_CTRL_VALUES(mMultiplierLimit, uvc_get_digital_multiplier_limit)
//    }
    RETURN(ret, int);
}

// digital multiplier limitを設定
int UVCCameraAdjustments::setDigitalMultiplierLimit(int multiplier_limit) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_DIGITAL_LIMIT) {
////		LOGI("multiplier limit:%d", multiplier_limit);
//        ret = internalSetCtrlValue(mMultiplierLimit, multiplier_limit, uvc_get_digital_multiplier_limit,
//                                   uvc_set_digital_multiplier_limit);
//    }
    RETURN(ret, int);
}

// digital multiplier limitの現在値を取得
int UVCCameraAdjustments::getDigitalMultiplierLimit() {

//    if (mPUSupports & PU_DIGITAL_LIMIT) {
//        int ret = update_ctrl_values(mDeviceHandle, mMultiplierLimit, uvc_get_digital_multiplier_limit);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            uint16_t multiplier_limit;
//            ret = uvc_get_digital_multiplier_limit(mDeviceHandle, &multiplier_limit, UVC_GET_CUR);
////			LOGI("multiplier_limit:%d", multiplier_limit);
//            if (LIKELY(!ret))
//                return multiplier_limit;
//        }
//    }
    RETURN(0, int);
}

//======================================================================
// AnalogVideoStandard
int UVCCameraAdjustments::updateAnalogVideoStandardLimit(int &min, int &max, int &def) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_AVIDEO_STD) {
//        UPDATE_CTRL_VALUES(mAnalogVideoStandard, uvc_get_analog_video_standard)
//    }
    RETURN(ret, int);
}

int UVCCameraAdjustments::setAnalogVideoStandard(int standard) {

    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_AVIDEO_STD) {
////		LOGI("standard:%d", standard);
//        ret = internalSetCtrlValue(mAnalogVideoStandard, standard, uvc_get_analog_video_standard,
//                                   uvc_set_analog_video_standard);
//    }
    RETURN(ret, int);
}

int UVCCameraAdjustments::getAnalogVideoStandard() {

//    if (mPUSupports & PU_AVIDEO_STD) {
//        int ret = update_ctrl_values(mDeviceHandle, mAnalogVideoStandard, uvc_get_analog_video_standard);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            uint8_t standard;
//            ret = uvc_get_analog_video_standard(mDeviceHandle, &standard, UVC_GET_CUR);
////			LOGI("standard:%d", standard);
//            if (LIKELY(!ret))
//                return standard;
//        }
//    }
    RETURN(0, int);
}

//======================================================================
// AnalogVideoLoackStatus
int UVCCameraAdjustments::updateAnalogVideoLockStateLimit(int &min, int &max, int &def) {
    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_AVIDEO_LOCK) {
//        UPDATE_CTRL_VALUES(mAnalogVideoLockState, uvc_get_analog_video_lock_status)
//    }
    RETURN(ret, int);
}

int UVCCameraAdjustments::setAnalogVideoLockState(int state) {
    int ret = UVC_ERROR_IO;
//    if (mPUSupports & PU_AVIDEO_LOCK) {
////		LOGI("status:%d", status);
//        ret = internalSetCtrlValue(mAnalogVideoLockState, state, uvc_get_analog_video_lock_status,
//                                   uvc_set_analog_video_lock_status);
//    }
    RETURN(ret, int);
}

int UVCCameraAdjustments::getAnalogVideoLockState() {
//    if (mPUSupports & PU_AVIDEO_LOCK) {
//        int ret = update_ctrl_values(mDeviceHandle, mAnalogVideoLockState, uvc_get_analog_video_lock_status);
//        if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
//            uint8_t status;
//            ret = uvc_get_analog_video_lock_status(mDeviceHandle, &status, UVC_GET_CUR);
////			LOGI("status:%d", status);
//            if (LIKELY(!ret))
//                return status;
//        }
//    }
    RETURN(0, int);
}


/**
 * カメラコントロール設定の下請け
 */
int UVCCameraAdjustments::internalSetCtrlValue(control_value_t &values, int8_t value,
                                               paramget_func_i8 get_func, paramset_func_i8 set_func) {
    int ret = update_ctrl_values(mDeviceHandle, values, get_func);
    if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
        value = value < values.min
                ? values.min
                : (value > values.max ? values.max : value);
        set_func(mDeviceHandle, value);
    }
    RETURN(ret, int);
}

int UVCCameraAdjustments::internalSetCtrlValue(control_value_t &values, uint8_t value,
                                               paramget_func_u8 get_func, paramset_func_u8 set_func) {
    int ret = update_ctrl_values(mDeviceHandle, values, get_func);
    if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
        value = value < values.min
                ? values.min
                : (value > values.max ? values.max : value);
        set_func(mDeviceHandle, value);
    }
    RETURN(ret, int);
}

int UVCCameraAdjustments::internalSetCtrlValue(control_value_t &values, uint8_t value1, uint8_t value2,
                                               paramget_func_u8u8 get_func, paramset_func_u8u8 set_func) {
    int ret = update_ctrl_values(mDeviceHandle, values, get_func);
    if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
        uint8_t v1min = (uint8_t) ((values.min >> 8) & 0xff);
        uint8_t v2min = (uint8_t) (values.min & 0xff);
        uint8_t v1max = (uint8_t) ((values.max >> 8) & 0xff);
        uint8_t v2max = (uint8_t) (values.max & 0xff);
        value1 = value1 < v1min
                 ? v1min
                 : (value1 > v1max ? v1max : value1);
        value2 = value2 < v2min
                 ? v2min
                 : (value2 > v2max ? v2max : value2);
        set_func(mDeviceHandle, value1, value2);
    }
    RETURN(ret, int);
}

int UVCCameraAdjustments::internalSetCtrlValue(control_value_t &values, int8_t value1, uint8_t value2,
                                               paramget_func_i8u8 get_func, paramset_func_i8u8 set_func) {
    int ret = update_ctrl_values(mDeviceHandle, values, get_func);
    if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
        int8_t v1min = (int8_t) ((values.min >> 8) & 0xff);
        uint8_t v2min = (uint8_t) (values.min & 0xff);
        int8_t v1max = (int8_t) ((values.max >> 8) & 0xff);
        uint8_t v2max = (uint8_t) (values.max & 0xff);
        value1 = value1 < v1min
                 ? v1min
                 : (value1 > v1max ? v1max : value1);
        value2 = value2 < v2min
                 ? v2min
                 : (value2 > v2max ? v2max : value2);
        set_func(mDeviceHandle, value1, value2);
    }
    RETURN(ret, int);
}

int UVCCameraAdjustments::internalSetCtrlValue(control_value_t &values, int8_t value1, uint8_t value2, uint8_t value3,
                                               paramget_func_i8u8u8 get_func, paramset_func_i8u8u8 set_func) {
    int ret = update_ctrl_values(mDeviceHandle, values, get_func);
    if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
        int8_t v1min = (int8_t) ((values.min >> 16) & 0xff);
        uint8_t v2min = (uint8_t) ((values.min >> 8) & 0xff);
        uint8_t v3min = (uint8_t) (values.min & 0xff);
        int8_t v1max = (int8_t) ((values.max >> 16) & 0xff);
        uint8_t v2max = (uint8_t) ((values.max >> 8) & 0xff);
        uint8_t v3max = (uint8_t) (values.max & 0xff);
        value1 = value1 < v1min
                 ? v1min
                 : (value1 > v1max ? v1max : value1);
        value2 = value2 < v2min
                 ? v2min
                 : (value2 > v2max ? v2max : value2);
        value3 = value3 < v3min
                 ? v3min
                 : (value3 > v3max ? v3max : value3);
        set_func(mDeviceHandle, value1, value2, value3);
    }
    RETURN(ret, int);
}

/**
 * カメラコントロール設定の下請け
 */
int UVCCameraAdjustments::internalSetCtrlValue(control_value_t &values, int16_t value,
                                               paramget_func_i16 get_func, paramset_func_i16 set_func) {
    int ret = update_ctrl_values(mDeviceHandle, values, get_func);
    if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
        value = value < values.min
                ? values.min
                : (value > values.max ? values.max : value);
        set_func(mDeviceHandle, value);
    }
    RETURN(ret, int);
}

/**
 * カメラコントロール設定の下請け
 */
int UVCCameraAdjustments::internalSetCtrlValue(control_value_t &values, uint16_t value,
                                               paramget_func_u16 get_func, paramset_func_u16 set_func) {
    int ret = update_ctrl_values(mDeviceHandle, values, get_func);
    if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
        value = value < values.min
                ? values.min
                : (value > values.max ? values.max : value);
        set_func(mDeviceHandle, value);
    }
    RETURN(ret, int);
}

/**
 * カメラコントロール設定の下請け
 */
int UVCCameraAdjustments::internalSetCtrlValue(control_value_t &values, int32_t value,
                                               paramget_func_i32 get_func, paramset_func_i32 set_func) {
    int ret = update_ctrl_values(mDeviceHandle, values, get_func);
    if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
        value = value < values.min
                ? values.min
                : (value > values.max ? values.max : value);
        set_func(mDeviceHandle, value);
    }
    RETURN(ret, int);
}

/**
 * カメラコントロール設定の下請け
 */
int UVCCameraAdjustments::internalSetCtrlValue(control_value_t &values, uint32_t value,
                                               paramget_func_u32 get_func, paramset_func_u32 set_func) {
    int ret = update_ctrl_values(mDeviceHandle, values, get_func);
    if (LIKELY(!ret)) {    // 正常に最小・最大値を取得出来た時
        value = value < values.min
                ? values.min
                : (value > values.max ? values.max : value);
        set_func(mDeviceHandle, value);
    }
    RETURN(ret, int);
}

/** Control UVC Camera with value check
 *
 * @param value
 * @return control result
 */
int UVCCameraAdjustments::internalSetCtrlValue(int32_t value, paramset_func_u16 set_func) {
    int ret = set_func(mDeviceHandle, value);
    RETURN(ret, int);
}