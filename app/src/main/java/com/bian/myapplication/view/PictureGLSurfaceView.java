package com.bian.myapplication.view;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

import com.bian.myapplication.utils.CommonLog;
import com.bian.myapplication.utils.NativePracise;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class PictureGLSurfaceView extends GLSurfaceView {
    public PictureGLSurfaceView(Context context) {
        this(context, null);
    }

    public PictureGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);
        NativePracise.initAsserts(context.getAssets());
        setRenderer(new MyRender());
        setRenderMode(RENDERMODE_WHEN_DIRTY);
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
    }

    private static class MyRender implements GLSurfaceView.Renderer {

        @Override
        public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
            CommonLog.i("onSurfaceCreated");
            NativePracise.init();
        }

        @Override
        public void onSurfaceChanged(GL10 gl10, int i, int i1) {
            CommonLog.i("onSurfaceChanged");
            NativePracise.resize(i, i1);
        }

        @Override
        public void onDrawFrame(GL10 gl10) {
            CommonLog.i("onDrawFrame");
            NativePracise.step();
        }
    }
}
