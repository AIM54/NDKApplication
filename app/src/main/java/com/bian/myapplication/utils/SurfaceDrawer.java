package com.bian.myapplication.utils;

import android.view.Surface;

public class SurfaceDrawer {

    private Surface mSurface;

    static {
        System.loadLibrary("NdkTest");
    }

    public SurfaceDrawer(Surface surface) {
        this.mSurface = surface;
    }

    public native void initSurfaceView(Surface surface);

    public native void resizeSurfaceView();

    public native void stepSurfaceView();

}
