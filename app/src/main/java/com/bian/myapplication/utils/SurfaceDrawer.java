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

    public SurfaceDrawer(Surface surface, AssetManager assets, int type) {
        this.mSurface = surface;
        this.assetManager = assets;
        initSurfaceView(mSurface, assetManager,type);
    }

    public native void initSurfaceView(Surface surface, AssetManager assetManager);

    public native void initSurfaceView(Surface surface, AssetManager assetManager, int testType);

    public native void resizeSurfaceView(int width, int height);

    public native void stepSurfaceView();

    public native void destroyView();
}
