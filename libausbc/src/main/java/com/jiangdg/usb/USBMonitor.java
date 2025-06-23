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
package com.jiangdg.usb;

import android.annotation.SuppressLint;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.os.Build;
import android.os.Handler;
import android.text.TextUtils;
import android.util.SparseArray;

import com.jiangdg.utils.BuildCheck;
import com.jiangdg.utils.HandlerThreadHandler;

import java.lang.ref.WeakReference;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import timber.log.Timber;

public final class USBMonitor {

    public static boolean DEBUG = true;    // TODO set false on production
    private static final String TAG = "USBMonitor";

    private static final String ACTION_USB_PERMISSION_BASE = "com.serenegiant.USB_PERMISSION.";
    private final String ACTION_USB_PERMISSION = ACTION_USB_PERMISSION_BASE + hashCode();

    /**
     * openしているUsbControlBlock
     */
    protected final ConcurrentHashMap<UsbDevice, UsbControlBlock> mCtrlBlocks = new ConcurrentHashMap<>();
    private final SparseArray<WeakReference<UsbDevice>> mHasPermissions = new SparseArray<>();
    private final WeakReference<Context> mWeakContext;
    protected final UsbManager mUsbManager;
    protected final OnDeviceConnectListener mOnDeviceConnectListener;
    private PendingIntent mPermissionIntent = null;

    /**
     * コールバックをワーカースレッドで呼び出すためのハンドラー
     */
    private final Handler mAsyncHandler;
    private volatile boolean destroyed;

    /**
     * USB機器の状態変更時のコールバックリスナー
     */
    public interface OnDeviceConnectListener {
        /**
         * called when device dettach(after onDisconnect)
         */
        void onDetach(UsbDevice device);

        /**
         * called after device opend
         */
        void onConnect(UsbDevice device, UsbControlBlock ctrlBlock, boolean createNew);

        /**
         * called when USB device removed or its power off (this callback is called after device closing)
         */
        void onDisconnect(UsbDevice device, UsbControlBlock ctrlBlock);

        /**
         * called when canceled or could not get permission from user
         */
        void onCancel(UsbDevice device);
    }

    public USBMonitor(final Context context, final OnDeviceConnectListener listener) {
        Timber.v("USBMonitor:Constructor");
        if (listener == null)
            throw new IllegalArgumentException("OnDeviceConnectListener should not null.");
        mWeakContext = new WeakReference<Context>(context);
        mUsbManager = (UsbManager) context.getSystemService(Context.USB_SERVICE);
        mOnDeviceConnectListener = listener;
        mAsyncHandler = HandlerThreadHandler.createHandler(TAG);
        destroyed = false;
        Timber.v("USBMonitor:mUsbManager=" + mUsbManager);
    }

    /**
     * Release all related resources,
     * never reuse again
     */
    public void destroy() {
        Timber.i("destroy:");
        unregister();
        if (!destroyed) {
            destroyed = true;
            // モニターしているUSB機器を全てcloseする
            final Set<UsbDevice> keys = mCtrlBlocks.keySet();
            UsbControlBlock ctrlBlock;
            try {
                for (final UsbDevice key : keys) {
                    ctrlBlock = mCtrlBlocks.remove(key);
                    if (ctrlBlock != null) {
                        ctrlBlock.close();
                    }
                }
            } catch (final Exception e) {
                Timber.e(e);
            }
            mCtrlBlocks.clear();
            try {
                mAsyncHandler.getLooper().quit();
            } catch (final Exception e) {
                Timber.e(e);
            }
        }
    }

    /**
     * register BroadcastReceiver to monitor USB events
     */
    @SuppressLint({"UnspecifiedImmutableFlag", "WrongConstant"})
    public synchronized void register() throws IllegalStateException {
        if (destroyed) throw new IllegalStateException("already destroyed");
        if (mPermissionIntent == null) {
            Timber.i("register:");
            final Context context = mWeakContext.get();
            if (context != null) {
                if (Build.VERSION.SDK_INT >= 31) {
                    mPermissionIntent = PendingIntent.getBroadcast(context, 0, new Intent(ACTION_USB_PERMISSION), PendingIntent.FLAG_IMMUTABLE);
                } else {
                    mPermissionIntent = PendingIntent.getBroadcast(context, 0, new Intent(ACTION_USB_PERMISSION), 0);
                }
                final IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
                filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
                context.registerReceiver(mUsbReceiver, filter, Context.RECEIVER_EXPORTED);
            }
            // start connection check
        }
    }

    /**
     * unregister BroadcastReceiver
     *
     * @throws IllegalStateException
     */
    public synchronized void unregister() throws IllegalStateException {
        // 接続チェック用Runnableを削除
        if (mPermissionIntent != null) {
            mPermissionIntent = null;
        }
    }

    public synchronized boolean isRegistered() {
        return !destroyed && (mPermissionIntent != null);
    }

    /**
     * 内部で保持しているパーミッション状態を更新
     *
     * @param device
     * @param hasPermission
     * @return hasPermission
     */
    private boolean updatePermission(final UsbDevice device, final boolean hasPermission) {
        // fix api >= 29, permission SecurityException
        try {
            final int deviceKey = getDeviceKey(device);
            synchronized (mHasPermissions) {
                if (hasPermission) {
                    if (mHasPermissions.get(deviceKey) == null) {
                        mHasPermissions.put(deviceKey, new WeakReference<UsbDevice>(device));
                    }
                } else {
                    mHasPermissions.remove(deviceKey);
                }
            }
        } catch (SecurityException e) {
            Timber.e(e);
        }

        return hasPermission;
    }

    /**
     * request permission to access to USB device
     *
     * @param device
     * @return true if fail to request permission
     */
    public synchronized boolean requestPermission(final UsbDevice device) {
        boolean result = false;
        if (isRegistered()) {
            if (device != null) {
                Timber.i("request permission, has permission: " + mUsbManager.hasPermission(device));
                if (mUsbManager.hasPermission(device)) {
                    // call onConnect if app already has permission
                    processConnect(device);
                } else {
                    try {
                        // パーミッションがなければ要求する
                        if (DEBUG) Timber.i("start request permission...");
                        mUsbManager.requestPermission(device, mPermissionIntent);
                    } catch (final Exception e) {
                        // Android5.1.xのGALAXY系でandroid.permission.sec.MDM_APP_MGMTという意味不明の例外生成するみたい
                        Timber.w("request permission failed, e = " + e.getLocalizedMessage(), e);
                        processCancel(device);
                        result = true;
                    }
                }
            } else {
                if (DEBUG)
                    Timber.w("request permission failed, device is null?");
                processCancel(device);
                result = true;
            }
        } else {
            if (DEBUG)
                Timber.w("request permission failed, not registered?");
            processCancel(device);
            result = true;
        }
        return result;
    }

    /**
     * BroadcastReceiver for USB permission
     */
    private final BroadcastReceiver mUsbReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(final Context context, final Intent intent) {
            if (destroyed) return;
            final String action = intent.getAction();
            if (ACTION_USB_PERMISSION.equals(action)) {
                // when received the result of requesting USB permission
                synchronized (USBMonitor.this) {
                    final UsbDevice device = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                    if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                        if (device != null) {
                            // get permission, call onConnect
                            if (DEBUG)
                                Timber.w("get permission success in mUsbReceiver");
                            processConnect(device);
                        }
                    } else {
                        // failed to get permission
                        if (DEBUG)
                            Timber.w("get permission failed in mUsbReceiver");
                        processCancel(device);
                    }
                }
            } else if (UsbManager.ACTION_USB_DEVICE_DETACHED.equals(action)) {
                // when device removed
                final UsbDevice device = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                if (device != null) {
                    UsbControlBlock ctrlBlock = mCtrlBlocks.remove(device);
                    if (ctrlBlock != null) {
                        // cleanup
                        ctrlBlock.close();
                    }
                    processDettach(device);
                }
            }
        }
    };

    /**
     * open specific USB device
     *
     * @param device UsbDevice
     */
    private void processConnect(final UsbDevice device) {
        if (destroyed) return;
        updatePermission(device, true);
        mAsyncHandler.post(() -> {
            Timber.v("processConnect:device=" + device.getDeviceName());
            UsbControlBlock ctrlBlock;
            final boolean createNew;
            ctrlBlock = mCtrlBlocks.get(device);
            if (ctrlBlock == null) {
                ctrlBlock = new UsbControlBlock(this, device);
                mCtrlBlocks.put(device, ctrlBlock);
                createNew = true;
            } else {
                createNew = false;
            }
            if (mOnDeviceConnectListener != null) {
                if (ctrlBlock.getConnection() == null) {
                    Timber.e("processConnect: Open device failed");
                    mOnDeviceConnectListener.onCancel(device);
                    return;
                }
                mOnDeviceConnectListener.onConnect(device, ctrlBlock, createNew);
            }
        });
    }

    private void processCancel(final UsbDevice device) {
        if (destroyed) return;
        Timber.v("processCancel:");
        updatePermission(device, false);
        if (mOnDeviceConnectListener != null) {
            mAsyncHandler.post(new Runnable() {
                @Override
                public void run() {
                    mOnDeviceConnectListener.onCancel(device);
                }
            });
        }
    }

    private void processDettach(final UsbDevice device) {
        if (destroyed) return;
        Timber.v("processDettach:");
        if (mOnDeviceConnectListener != null) {
            mAsyncHandler.post(new Runnable() {
                @Override
                public void run() {
                    mOnDeviceConnectListener.onDetach(device);
                }
            });
        }
    }

    /**
     * USB機器毎の設定保存用にデバイスキー名を生成する。この機器名をHashMapのキーにする
     * UsbDeviceがopenしている時のみ有効
     * ベンダーID, プロダクトID, デバイスクラス, デバイスサブクラス, デバイスプロトコルから生成
     * serialがnullや空文字でなければserialを含めたデバイスキー名を生成する
     * useNewAPI=trueでAPIレベルを満たしていればマニュファクチャ名, バージョン, コンフィギュレーションカウントも使う
     *
     * @param device    nullなら空文字列を返す
     * @param serial    UsbDeviceConnection#getSerialで取得したシリアル番号を渡す, nullでuseNewAPI=trueでAPI>=21なら内部で取得
     */
    @SuppressLint("NewApi")
    public String getDeviceKeyName(final UsbDevice device, final String serial) {
        if (device == null) return "";
        final StringBuilder sb = new StringBuilder();
        sb.append(device.getVendorId());
        sb.append("#");    // API >= 12
        sb.append(device.getProductId());
        sb.append("#");    // API >= 12
        sb.append(device.getDeviceClass());
        sb.append("#");    // API >= 12
        sb.append(device.getDeviceSubclass());
        sb.append("#");    // API >= 12
        sb.append(device.getDeviceProtocol());                        // API >= 12
        if (!TextUtils.isEmpty(serial)) {
            sb.append("#");
            sb.append(serial);
        }
        sb.append("#");
        if (TextUtils.isEmpty(serial)) {
            try {
                sb.append(device.getSerialNumber());
                sb.append("#");
            } // API >= 21 & targetSdkVersion has to be <= 28
            catch (SecurityException ignore) {
            }
        }
        sb.append(device.getManufacturerName());
        sb.append("#");    // API >= 21
        sb.append(device.getConfigurationCount());
        sb.append("#");    // API >= 21
        if (BuildCheck.isMarshmallow()) {
            sb.append(device.getVersion());
            sb.append("#");    // API >= 23
        }
        return sb.toString();
    }

    /**
     * デバイスキーを整数として取得
     * getDeviceKeyNameで得られる文字列のhasCodeを取得
     * useNewAPI=falseで同種の製品だと同じデバイスキーになるので注意
     */
    public int getDeviceKey(final UsbDevice device) {
        return device != null ? getDeviceKeyName(device, null).hashCode() : 0;
    }

}
