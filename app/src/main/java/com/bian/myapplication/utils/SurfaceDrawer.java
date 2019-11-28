package com.bian.myapplication.utils;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.view.Surface;

public class SurfaceDrawer {

    private Surface mSurface;

    private AssetManager assetManager;

    static {
        System.loadLibrary("jpeg");
        System.loadLibrary("turbojpeg");
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
        initSurfaceView(mSurface, assetManager, type);
    }

    public SurfaceDrawer(Surface surface, AssetManager assets, int type, String pictureUrl) {
        this.mSurface = surface;
        this.assetManager = assets;
        initPicture(mSurface, assetManager, type, pictureUrl);
    }

    public SurfaceDrawer(Surface surface, AssetManager assets, int type, Bitmap trueBitmap) {
        this.mSurface = surface;
        this.assetManager = assets;
        initPicture(surface, assetManager, type, trueBitmap);
    }

    public native void initSurfaceView(Surface surface, AssetManager assetManager);

    public native void initSurfaceView(Surface surface, AssetManager assetManager, int testType);

    public native void initPicture(Surface surface, AssetManager assetManager, int testType, String pictureUrl);

    public native void initPicture(Surface surface, AssetManager assetManager, int testType, Bitmap bitmap);

    public native void resizeSurfaceView(int width, int height);

    public native void stepSurfaceView();

    public native void destroyView();
}
