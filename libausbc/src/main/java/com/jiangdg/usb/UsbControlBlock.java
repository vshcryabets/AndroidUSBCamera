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

import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbInterface;
import android.text.TextUtils;
import android.util.SparseArray;

import java.lang.ref.WeakReference;
import java.util.Locale;

import timber.log.Timber;

/**
 * control class
 * never reuse the instance when it closed
 */
public class UsbControlBlock implements Cloneable {
    private final WeakReference<USBMonitor> mWeakMonitor;
    private final WeakReference<UsbDevice> mWeakDevice;
    protected UsbDeviceConnection mConnection;
    private final int mBusNum;
    private final int mDevNum;
    private final SparseArray<SparseArray<UsbInterface>> mInterfaces = new SparseArray<SparseArray<UsbInterface>>();

    /**
     * this class needs permission to access USB device before constructing
     *
     * @param monitor
     * @param device
     */
    protected UsbControlBlock(final USBMonitor monitor, final UsbDevice device) {
        if (USBMonitor.DEBUG) Timber.i("UsbControlBlock:constructor");
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
        final String name = device.getDeviceName();
        final String[] v = !TextUtils.isEmpty(name) ? name.split("/") : null;
        int busnum = 0;
        int devnum = 0;
        if (v != null) {
            busnum = Integer.parseInt(v[v.length - 2]);
            devnum = Integer.parseInt(v[v.length - 1]);
        }
        mBusNum = busnum;
        mDevNum = devnum;
        if (USBMonitor.DEBUG) {
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
     *
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
     *
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
     */
    public String getDeviceName() {
        final UsbDevice device = mWeakDevice.get();
        return device != null ? device.getDeviceName() : "";
    }

    /**
     * get UsbDeviceConnection
     */
    public synchronized UsbDeviceConnection getConnection() {
        return mConnection;
    }

    /**
     * get file descriptor to access USB device
     */
    public synchronized int getFileDescriptor() throws IllegalStateException {
        checkConnection();
        return mConnection.getFileDescriptor();
    }

    /**
     * get vendor id
     */
    public int getVenderId() {
        final UsbDevice device = mWeakDevice.get();
        return device != null ? device.getVendorId() : 0;
    }

    /**
     * get product id
     */
    public int getProductId() {
        final UsbDevice device = mWeakDevice.get();
        return device != null ? device.getProductId() : 0;
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
        if (USBMonitor.DEBUG) Timber.i("UsbControlBlock#close:");

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