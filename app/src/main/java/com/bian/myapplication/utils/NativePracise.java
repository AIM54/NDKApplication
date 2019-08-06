package com.bian.myapplication.utils;

import java.util.ArrayList;

public class NativePracise {
    static {
        System.loadLibrary("NdkTest");
    }

    public static native void test();

    public static native ArrayList<String> testTemple(int size,String message);
}
