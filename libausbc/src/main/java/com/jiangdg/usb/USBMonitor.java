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
import android.annotation.TargetApi;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;
import android.os.Build;
import android.os.Handler;
import android.text.TextUtils;
import android.util.SparseArray;

import com.jiangdg.utils.BuildCheck;
import com.jiangdg.utils.HandlerThreadHandler;

import java.io.UnsupportedEncodingException;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import timber.log.Timber;

public final class USBMonitor {

	public static boolean DEBUG = true;	// TODO set false on production
	private static final String TAG = "USBMonitor";

	private static final String ACTION_USB_PERMISSION_BASE = "com.serenegiant.USB_PERMISSION.";
	private final String ACTION_USB_PERMISSION = ACTION_USB_PERMISSION_BASE + hashCode();

	/**
	 * openしているUsbControlBlock
	 */
	private final ConcurrentHashMap<UsbDevice, UsbControlBlock> mCtrlBlocks = new ConcurrentHashMap<UsbDevice, UsbControlBlock>();
	private final SparseArray<WeakReference<UsbDevice>> mHasPermissions = new SparseArray<WeakReference<UsbDevice>>();

	private final WeakReference<Context> mWeakContext;
	private final UsbManager mUsbManager;
	private final OnDeviceConnectListener mOnDeviceConnectListener;
	private PendingIntent mPermissionIntent = null;
	private List<DeviceFilter> mDeviceFilters = new ArrayList<DeviceFilter>();

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
		 * @param device
		 */
		public void onDetach(UsbDevice device);
		/**
		 * called after device opend
		 * @param device
		 * @param ctrlBlock
		 * @param createNew
		 */
		public void onConnect(UsbDevice device, UsbControlBlock ctrlBlock, boolean createNew);
		/**
		 * called when USB device removed or its power off (this callback is called after device closing)
		 * @param device
		 * @param ctrlBlock
		 */
		public void onDisconnect(UsbDevice device, UsbControlBlock ctrlBlock);
		/**
		 * called when canceled or could not get permission from user
		 * @param device
		 */
		public void onCancel(UsbDevice device);
	}

	public USBMonitor(final Context context, final OnDeviceConnectListener listener) {
		Timber.v("USBMonitor:Constructor");
		if (listener == null)
			throw new IllegalArgumentException("OnDeviceConnectListener should not null.");
		mWeakContext = new WeakReference<Context>(context);
		mUsbManager = (UsbManager)context.getSystemService(Context.USB_SERVICE);
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
			if (keys != null) {
				UsbControlBlock ctrlBlock;
				try {
					for (final UsbDevice key: keys) {
						ctrlBlock = mCtrlBlocks.remove(key);
						if (ctrlBlock != null) {
							ctrlBlock.close();
						}
					}
				} catch (final Exception e) {
					Timber.e(e);
				}
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
	 * @throws IllegalStateException
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
			mDeviceCounts = 0;
		}
	}

	/**
	 * unregister BroadcastReceiver
	 * @throws IllegalStateException
	 */
	public synchronized void unregister() throws IllegalStateException {
		// 接続チェック用Runnableを削除
		mDeviceCounts = 0;
		if (mPermissionIntent != null) {
			mPermissionIntent = null;
		}
	}

	public synchronized boolean isRegistered() {
		return !destroyed && (mPermissionIntent != null);
	}

	/**
	 * 内部で保持しているパーミッション状態を更新
	 * @param device
	 * @param hasPermission
	 * @return hasPermission
	 */
	private boolean updatePermission(final UsbDevice device, final boolean hasPermission) {
		// fix api >= 29, permission SecurityException
		try {
			final int deviceKey = getDeviceKey(device, true);
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
	 * @param device
	 * @return true if fail to request permission
	 */
	public synchronized boolean requestPermission(final UsbDevice device) {
//		Timber.v("requestPermission:device=" + device);
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
						Timber.w("request permission failed, e = " + e.getLocalizedMessage() ,e);
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
					mDeviceCounts = 0;
					processDettach(device);
				}
			}
		}
	};

	/** number of connected & detected devices */
	private volatile int mDeviceCounts = 0;
	/**
	 * open specific USB device
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
				ctrlBlock = new UsbControlBlock(USBMonitor.this, device);
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
	 * @param device nullなら空文字列を返す
	 * @param serial	UsbDeviceConnection#getSerialで取得したシリアル番号を渡す, nullでuseNewAPI=trueでAPI>=21なら内部で取得
	 * @param useNewAPI API>=21またはAPI>=23のみで使用可能なメソッドも使用する(ただし機器によってはnullが返ってくるので有効かどうかは機器による)
	 * @return
	 */
	@SuppressLint("NewApi")
	public static final String getDeviceKeyName(final UsbDevice device, final String serial, final boolean useNewAPI) {
		if (device == null) return "";
		final StringBuilder sb = new StringBuilder();
		sb.append(device.getVendorId());			sb.append("#");	// API >= 12
		sb.append(device.getProductId());			sb.append("#");	// API >= 12
		sb.append(device.getDeviceClass());			sb.append("#");	// API >= 12
		sb.append(device.getDeviceSubclass());		sb.append("#");	// API >= 12
		sb.append(device.getDeviceProtocol());						// API >= 12
		if (!TextUtils.isEmpty(serial)) {
			sb.append("#");	sb.append(serial);
		}
		if (useNewAPI && BuildCheck.isAndroid5()) {
			sb.append("#");
			if (TextUtils.isEmpty(serial)) {
				try { sb.append(device.getSerialNumber());	sb.append("#");	} // API >= 21 & targetSdkVersion has to be <= 28
				catch(SecurityException ignore) {}
			}
			sb.append(device.getManufacturerName());	sb.append("#");	// API >= 21
			sb.append(device.getConfigurationCount());	sb.append("#");	// API >= 21
			if (BuildCheck.isMarshmallow()) {
				sb.append(device.getVersion());			sb.append("#");	// API >= 23
			}
		}
//		Timber.v("getDeviceKeyName:" + sb.toString());
		return sb.toString();
	}

	/**
	 * デバイスキーを整数として取得
	 * getDeviceKeyNameで得られる文字列のhasCodeを取得
	 * useNewAPI=falseで同種の製品だと同じデバイスキーになるので注意
	 * @param device
	 * @param useNewAPI
	 * @return
	 */
	public static final int getDeviceKey(final UsbDevice device, final boolean useNewAPI) {
		return device != null ? getDeviceKeyName(device, null, useNewAPI).hashCode() : 0;
	}

	public static class UsbDeviceInfo {
		public String usb_version;
		public String manufacturer;
		public String product;
		public String version;
		public String serial;

		private void clear() {
			usb_version = manufacturer = product = version = serial = null;
		}

		@Override
		public String toString() {
			return String.format("UsbDevice:usb_version=%s,manufacturer=%s,product=%s,version=%s,serial=%s",
				usb_version != null ? usb_version : "",
				manufacturer != null ? manufacturer : "",
				product != null ? product : "",
				version != null ? version : "",
				serial != null ? serial : "");
		}
	}

	private static final int USB_DIR_OUT = 0;
	private static final int USB_DIR_IN = 0x80;
	private static final int USB_TYPE_MASK = (0x03 << 5);
	private static final int USB_TYPE_STANDARD = (0x00 << 5);
	private static final int USB_TYPE_CLASS = (0x01 << 5);
	private static final int USB_TYPE_VENDOR = (0x02 << 5);
	private static final int USB_TYPE_RESERVED = (0x03 << 5);
	private static final int USB_RECIP_MASK = 0x1f;
	private static final int USB_RECIP_DEVICE = 0x00;
	private static final int USB_RECIP_INTERFACE = 0x01;
	private static final int USB_RECIP_ENDPOINT = 0x02;
	private static final int USB_RECIP_OTHER = 0x03;
	private static final int USB_RECIP_PORT = 0x04;
	private static final int USB_RECIP_RPIPE = 0x05;
	private static final int USB_REQ_GET_STATUS = 0x00;
	private static final int USB_REQ_CLEAR_FEATURE = 0x01;
	private static final int USB_REQ_SET_FEATURE = 0x03;
	private static final int USB_REQ_SET_ADDRESS = 0x05;
	private static final int USB_REQ_GET_DESCRIPTOR = 0x06;
	private static final int USB_REQ_SET_DESCRIPTOR = 0x07;
	private static final int USB_REQ_GET_CONFIGURATION = 0x08;
	private static final int USB_REQ_SET_CONFIGURATION = 0x09;
	private static final int USB_REQ_GET_INTERFACE = 0x0A;
	private static final int USB_REQ_SET_INTERFACE = 0x0B;
	private static final int USB_REQ_SYNCH_FRAME = 0x0C;
	private static final int USB_REQ_SET_SEL = 0x30;
	private static final int USB_REQ_SET_ISOCH_DELAY = 0x31;
	private static final int USB_REQ_SET_ENCRYPTION = 0x0D;
	private static final int USB_REQ_GET_ENCRYPTION = 0x0E;
	private static final int USB_REQ_RPIPE_ABORT = 0x0E;
	private static final int USB_REQ_SET_HANDSHAKE = 0x0F;
	private static final int USB_REQ_RPIPE_RESET = 0x0F;
	private static final int USB_REQ_GET_HANDSHAKE = 0x10;
	private static final int USB_REQ_SET_CONNECTION = 0x11;
	private static final int USB_REQ_SET_SECURITY_DATA = 0x12;
	private static final int USB_REQ_GET_SECURITY_DATA = 0x13;
	private static final int USB_REQ_SET_WUSB_DATA = 0x14;
	private static final int USB_REQ_LOOPBACK_DATA_WRITE = 0x15;
	private static final int USB_REQ_LOOPBACK_DATA_READ = 0x16;
	private static final int USB_REQ_SET_INTERFACE_DS = 0x17;

	private static final int USB_REQ_STANDARD_DEVICE_SET = (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE);		// 0x10
	private static final int USB_REQ_STANDARD_DEVICE_GET = (USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_DEVICE);			// 0x90
	private static final int USB_REQ_STANDARD_INTERFACE_SET = (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_INTERFACE);	// 0x11
	private static final int USB_REQ_STANDARD_INTERFACE_GET = (USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_INTERFACE);	// 0x91
	private static final int USB_REQ_STANDARD_ENDPOINT_SET = (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_ENDPOINT);	// 0x12
	private static final int USB_REQ_STANDARD_ENDPOINT_GET = (USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_ENDPOINT);		// 0x92

	private static final int USB_REQ_CS_DEVICE_SET  = (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_DEVICE);				// 0x20
	private static final int USB_REQ_CS_DEVICE_GET = (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_DEVICE);					// 0xa0
	private static final int USB_REQ_CS_INTERFACE_SET = (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE);			// 0x21
	private static final int USB_REQ_CS_INTERFACE_GET = (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE);			// 0xa1
	private static final int USB_REQ_CS_ENDPOINT_SET = (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_ENDPOINT);				// 0x22
	private static final int USB_REQ_CS_ENDPOINT_GET = (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_ENDPOINT);				// 0xa2

	private static final int USB_REQ_VENDER_DEVICE_SET = (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_DEVICE);				// 0x40
	private static final int USB_REQ_VENDER_DEVICE_GET = (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_DEVICE);				// 0xc0
	private static final int USB_REQ_VENDER_INTERFACE_SET = (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE);		// 0x41
	private static final int USB_REQ_VENDER_INTERFACE_GET = (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE);		// 0xc1
	private static final int USB_REQ_VENDER_ENDPOINT_SET = (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_ENDPOINT);			// 0x42
	private static final int USB_REQ_VENDER_ENDPOINT_GET = (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_ENDPOINT);			// 0xc2

	private static final int USB_DT_DEVICE = 0x01;
	private static final int USB_DT_CONFIG = 0x02;
	private static final int USB_DT_STRING = 0x03;
	private static final int USB_DT_INTERFACE = 0x04;
	private static final int USB_DT_ENDPOINT = 0x05;

	/**
	 * 指定したIDのStringディスクリプタから文字列を取得する。取得できなければnull
	 * @param connection
	 * @param id
	 * @param languageCount
	 * @param languages
	 * @return
	 */
	private static String getString(final UsbDeviceConnection connection, final int id, final int languageCount, final byte[] languages) {
		final byte[] work = new byte[256];
		String result = null;
		for (int i = 1; i <= languageCount; i++) {
			int ret = connection.controlTransfer(
				USB_REQ_STANDARD_DEVICE_GET, // USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_DEVICE
				USB_REQ_GET_DESCRIPTOR,
				(USB_DT_STRING << 8) | id, languages[i], work, 256, 0);
			if ((ret > 2) && (work[0] == ret) && (work[1] == USB_DT_STRING)) {
				// skip first two bytes(bLength & bDescriptorType), and copy the rest to the string
				try {
					result = new String(work, 2, ret - 2, "UTF-16LE");
					if (!"Љ".equals(result)) {	// 変なゴミが返ってくる時がある
						break;
					} else {
						result = null;
					}
				} catch (final UnsupportedEncodingException e) {
					// ignore
				}
			}
		}
		return result;
	}

	/**
	 * ベンダー名・製品名・バージョン・シリアルを取得する
	 * @param manager
	 * @param device
	 * @param _info
	 * @return
	 */
	@TargetApi(Build.VERSION_CODES.M)
	public static UsbDeviceInfo updateDeviceInfo(final UsbManager manager, final UsbDevice device, final UsbDeviceInfo _info) {
		final UsbDeviceInfo info = _info != null ? _info : new UsbDeviceInfo();
		info.clear();

		if (device != null) {
			if (BuildCheck.isLollipop()) {
				info.manufacturer = device.getManufacturerName();
				info.product = device.getProductName();
				info.serial = device.getSerialNumber();
			}
			if (BuildCheck.isMarshmallow()) {
				info.usb_version = device.getVersion();
			}
			if ((manager != null) && manager.hasPermission(device)) {
				final UsbDeviceConnection connection = manager.openDevice(device);
				if(connection == null) {
					return null;
				}
				final byte[] desc = connection.getRawDescriptors();

				if (TextUtils.isEmpty(info.usb_version)) {
					info.usb_version = String.format("%x.%02x", ((int)desc[3] & 0xff), ((int)desc[2] & 0xff));
				}
				if (TextUtils.isEmpty(info.version)) {
					info.version = String.format("%x.%02x", ((int)desc[13] & 0xff), ((int)desc[12] & 0xff));
				}
				if (TextUtils.isEmpty(info.serial)) {
					info.serial = connection.getSerial();
				}

				final byte[] languages = new byte[256];
				int languageCount = 0;
				// controlTransfer(int requestType, int request, int value, int index, byte[] buffer, int length, int timeout)
				try {
					int result = connection.controlTransfer(
						USB_REQ_STANDARD_DEVICE_GET, // USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_DEVICE
	    				USB_REQ_GET_DESCRIPTOR,
	    				(USB_DT_STRING << 8) | 0, 0, languages, 256, 0);
					if (result > 0) {
	        			languageCount = (result - 2) / 2;
					}
					if (languageCount > 0) {
						if (TextUtils.isEmpty(info.manufacturer)) {
							info.manufacturer = getString(connection, desc[14], languageCount, languages);
						}
						if (TextUtils.isEmpty(info.product)) {
							info.product = getString(connection, desc[15], languageCount, languages);
						}
						if (TextUtils.isEmpty(info.serial)) {
							info.serial = getString(connection, desc[16], languageCount, languages);
						}
					}
				} finally {
					connection.close();
				}
			}
			if (TextUtils.isEmpty(info.manufacturer)) {
				info.manufacturer = USBVendorId.INSTANCE.vendorName(device.getVendorId());
			}
			if (TextUtils.isEmpty(info.manufacturer)) {
				info.manufacturer = String.format("%04x", device.getVendorId());
			}
			if (TextUtils.isEmpty(info.product)) {
				info.product = String.format("%04x", device.getProductId());
			}
		}
		return info;
	}

	/**
	 * control class
	 * never reuse the instance when it closed
	 */
	public static final class UsbControlBlock implements Cloneable {
		private final WeakReference<USBMonitor> mWeakMonitor;
		private final WeakReference<UsbDevice> mWeakDevice;
		protected UsbDeviceConnection mConnection;
		protected final UsbDeviceInfo mInfo;
		private final int mBusNum;
		private final int mDevNum;
		private final SparseArray<SparseArray<UsbInterface>> mInterfaces = new SparseArray<SparseArray<UsbInterface>>();

		/**
		 * this class needs permission to access USB device before constructing
		 * @param monitor
		 * @param device
		 */
		private UsbControlBlock(final USBMonitor monitor, final UsbDevice device) {
			if (DEBUG) Timber.i("UsbControlBlock:constructor");
			mWeakMonitor = new WeakReference<USBMonitor>(monitor);
			mWeakDevice = new WeakReference<UsbDevice>(device);
			mConnection = monitor.mUsbManager.openDevice(device);
			if (mConnection == null) {
				Timber.w("openDevice failed in UsbControlBlock11, wait and try again");
				try {
					Thread.sleep(500);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				mConnection = monitor.mUsbManager.openDevice(device);
			}
			mInfo = updateDeviceInfo(monitor.mUsbManager, device, null);
			final String name = device.getDeviceName();
			final String[] v = !TextUtils.isEmpty(name) ? name.split("/") : null;
			int busnum = 0;
			int devnum = 0;
			if (v != null) {
				busnum = Integer.parseInt(v[v.length-2]);
				devnum = Integer.parseInt(v[v.length-1]);
			}
			mBusNum = busnum;
			mDevNum = devnum;
			if (DEBUG) {
				if (mConnection != null) {
					final int desc = mConnection.getFileDescriptor();
					final byte[] rawDesc = mConnection.getRawDescriptors();
					Timber.i(String.format(Locale.US, "name=%s,desc=%d,busnum=%d,devnum=%d,rawDesc=", name, desc, busnum, devnum));
				} else {
					Timber.e("could not connect to device(mConnection=null) " + name);
				}
			}
		}

		/**
		 * copy constructor
		 * @param src
		 * @throws IllegalStateException
		 */
		private UsbControlBlock(final UsbControlBlock src) throws IllegalStateException {
			final USBMonitor monitor = src.getUSBMonitor();
			final UsbDevice device = src.getDevice();
			if (device == null) {
				throw new IllegalStateException("device may already be removed");
			}
			mConnection = monitor.mUsbManager.openDevice(device);
			if (mConnection == null) {
				Timber.w("openDevice failed in UsbControlBlock, wait and try again");
				try {
					Thread.sleep(500);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				mConnection = monitor.mUsbManager.openDevice(device);
				if (mConnection == null) {
					throw new IllegalStateException("openDevice failed. device may already be removed or have no permission, dev = " + device);
				}
			}
			mInfo = updateDeviceInfo(monitor.mUsbManager, device, null);
			mWeakMonitor = new WeakReference<USBMonitor>(monitor);
			mWeakDevice = new WeakReference<UsbDevice>(device);
			mBusNum = src.mBusNum;
			mDevNum = src.mDevNum;
			// FIXME USBMonitor.mCtrlBlocksに追加する(今はHashMapなので追加すると置き換わってしまうのでだめ, ListかHashMapにListをぶら下げる?)
		}

		/**
		 * duplicate by clone
		 * need permission
		 * USBMonitor never handle cloned UsbControlBlock, you should release it after using it.
		 * @return
		 * @throws CloneNotSupportedException
		 */
		@Override
		public UsbControlBlock clone() throws CloneNotSupportedException {
			final UsbControlBlock ctrlblock;
			try {
				ctrlblock = new UsbControlBlock(this);
			} catch (final IllegalStateException e) {
				throw new CloneNotSupportedException(e.getMessage());
			}
			return ctrlblock;
		}

		public USBMonitor getUSBMonitor() {
			return mWeakMonitor.get();
		}

		public UsbDevice getDevice() {
			return mWeakDevice.get();
		}

		/**
		 * get device name
		 * @return
		 */
		public String getDeviceName() {
			final UsbDevice device = mWeakDevice.get();
			return device != null ? device.getDeviceName() : "";
		}

		/**
		 * get UsbDeviceConnection
		 * @return
		 */
		public synchronized UsbDeviceConnection getConnection() {
			return mConnection;
		}

		/**
		 * get file descriptor to access USB device
		 * @return
		 * @throws IllegalStateException
		 */
		public synchronized int getFileDescriptor() throws IllegalStateException {
			checkConnection();
			return mConnection.getFileDescriptor();
		}

		/**
		 * get vendor id
		 * @return
		 */
		public int getVenderId() {
			final UsbDevice device = mWeakDevice.get();
			return device != null ? device.getVendorId() : 0;
		}

		/**
		 * get product id
		 * @return
		 */
		public int getProductId() {
			final UsbDevice device = mWeakDevice.get();
			return device != null ? device.getProductId() : 0;
		}

		/**
		 * get version
		 * @return
		 */
		public String getVersion() {
			return mInfo.version;
		}

		/**
		 * get serial number
		 * @return
		 */
		public String getSerial() {
			return mInfo.serial;
		}

		public int getBusNum() {
			return mBusNum;
		}

		public int getDevNum() {
			return mDevNum;
		}

		/**
		 * Close device
		 * This also close interfaces if they are opened in Java side
		 */
		public synchronized void close() {
			if (DEBUG) Timber.i("UsbControlBlock#close:");

			if (mConnection != null) {
				final int n = mInterfaces.size();
				for (int i = 0; i < n; i++) {
					final SparseArray<UsbInterface> intfs = mInterfaces.valueAt(i);
					if (intfs != null) {
						final int m = intfs.size();
						for (int j = 0; j < m; j++) {
							final UsbInterface intf = intfs.valueAt(j);
							mConnection.releaseInterface(intf);
						}
						intfs.clear();
					}
				}
				mInterfaces.clear();
				mConnection.close();
				mConnection = null;
				final USBMonitor monitor = mWeakMonitor.get();
				if (monitor != null) {
					if (monitor.mOnDeviceConnectListener != null) {
						monitor.mOnDeviceConnectListener.onDisconnect(mWeakDevice.get(), UsbControlBlock.this);
					}
					monitor.mCtrlBlocks.remove(getDevice());
				}
			}
		}

		@Override
		public boolean equals(final Object o) {
			if (o == null) return false;
			if (o instanceof UsbControlBlock) {
				final UsbDevice device = ((UsbControlBlock) o).getDevice();
				return device == null ? mWeakDevice.get() == null
						: device.equals(mWeakDevice.get());
			} else if (o instanceof UsbDevice) {
				return o.equals(mWeakDevice.get());
			}
			return super.equals(o);
		}
		private synchronized void checkConnection() throws IllegalStateException {
			if (mConnection == null) {
				throw new IllegalStateException("already closed");
			}
		}
	}

}
