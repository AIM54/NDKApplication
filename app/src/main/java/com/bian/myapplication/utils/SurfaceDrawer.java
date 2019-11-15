package com.bian.myapplication.utils;

import android.content.res.AssetManager;
import android.view.Surface;

public class SurfaceDrawer {

    private Surface mSurface;

    private AssetManager assetManager;

    static {
        System.loadLibrary("NdkTest");
    }

    public SurfaceDrawer(Surface surface, AssetManager assets) {
        this.mSurface = surface;
        this.assetManager = assets;
        initSurfaceView(mSurface, assetManager);
    }

    public native void initSurfaceView(Surface surface, AssetManager assetManager);

    public native void resizeSurfaceView(int width,int height);

    public native void stepSurfaceView();

    public native void destroyView();
}
