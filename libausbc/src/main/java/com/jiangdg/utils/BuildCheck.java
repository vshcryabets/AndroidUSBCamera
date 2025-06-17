package com.jiangdg.utils;
/*
 * libcommon
 * utility/helper classes for myself
 *
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
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
 */

import android.os.Build;

public final class BuildCheck {

    private static boolean check(final int value) {
        return (Build.VERSION.SDK_INT >= value);
    }

    /**
     * October 2008: The original, first, version of Android.  Yay!, API>=1
     */
    public static boolean isBase() {
        return check(Build.VERSION_CODES.BASE);
    }

    /**
     * Lollipop.  A flat one with beautiful shadows.  But still tasty., API>=21
     */
    public static boolean isL() {
        return (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP);
    }

    /**
     * Lollipop.  A flat one with beautiful shadows.  But still tasty., API>=21
     */
    public static boolean isLollipop() {
        return (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP);
    }

    /**
     * Lollipop.  A flat one with beautiful shadows.  But still tasty., API>=21
     */
    public static boolean isAndroid5() {
        return check(Build.VERSION_CODES.LOLLIPOP);
    }

    /**
     * Lollipop with an extra sugar coating on the outside!, API>=22
     */
    public static boolean isLollipopMR1() {
        return (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP_MR1);
    }

    /**
     * Marshmallow.  A flat one with beautiful shadows.  But still tasty., API>=23
     */
    public static boolean isM() {
        return check(Build.VERSION_CODES.M);
    }

    /**
     * Marshmallow.  A flat one with beautiful shadows.  But still tasty., API>=23
     * @return
     */
    public static boolean isMarshmallow() {
        return check(Build.VERSION_CODES.M);
    }

    /**
     * Marshmallow.  A flat one with beautiful shadows.  But still tasty., API>=23
     * @return
     */
    public static boolean isAndroid6() {
        return check(Build.VERSION_CODES.M);
    }

    /**
     * 虫歯の元, API >= 24
     * @return
     */
    public static boolean isN() {
        return check(Build.VERSION_CODES.N);
    }

    /**
     * 歯にくっつくやつ, API >= 24
     * @return
     */
    public static boolean isNougat() {
        return check(Build.VERSION_CODES.N);
    }
    /**
     * API >= 24
     * @return
     */
    public static boolean isAndroid7() {
        return check(Build.VERSION_CODES.N);
    }

    /**
     * API>=25
     * @return
     */
    public static boolean isNMR1() {
        return check(Build.VERSION_CODES.N_MR1);
    }

    /**
     * API>=25
     * @return
     */
    public static boolean isNougatMR1() {
        return check(Build.VERSION_CODES.N_MR1);
    }

    /**
     * おれおれぇー API>=26
     * @return
     */
    public static boolean isO() {
        return check(Build.VERSION_CODES.O);
    }

    /**
     * おっ！ぱい API>=28
     * @return
     */
    public static boolean isP() {
        return check((Build.VERSION_CODES.P));
    }

}
