package com.bian.myapplication.view;

import android.content.Context;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class MySurfaceView extends SurfaceView implements SurfaceHolder.Callback {

    private MyRenderThread myRenderThread;

    private MyRender mRender;

    public void setRender(MyRender render) {
        this.mRender = render;
        myRenderThread = new MyRenderThread();
        myRenderThread.start();
    }

    public interface MyRender {
        void init(Surface surface);

        void onSizeChanged(int width, int height);

        void draw();

        void destroy();
    }

    public MySurfaceView(Context context) {
        this(context, null);
    }

    public MySurfaceView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public MySurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        getHolder().addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        myRenderThread.surfaceCreated(holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        myRenderThread.onSizeChanged(width, height);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        myRenderThread.onDestory();
    }

    public class MyRenderThread extends Thread {
        private boolean shallExit;
        private int viewWidth, viewHeight;
        private Surface mSurface;
        private boolean hasInit;
        private boolean windowHasChanged;
        private RenderManager renderManager;


        public MyRenderThread() {
            shallExit = false;
            renderManager = new RenderManager();
        }

        @Override
        public void run() {
            try {
                runRenderTask();
                mRender.destroy();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        private void runRenderTask() throws InterruptedException {
            while (!shallExit) {

                if (mSurface == null && !shallExit) {
                    synchronized (renderManager) {
                        while (mSurface == null) {
                            renderManager.wait();
                        }
                    }
                }
                if (!hasInit) {
                    mRender.init(mSurface);
                    hasInit = true;
                }
                if (viewWidth <= 0 && viewHeight <= 0) {
                    synchronized (renderManager) {
                        while (viewWidth <= 0 && viewHeight <= 0) {
                            renderManager.wait();
                        }
                    }
                }

                if (windowHasChanged && mRender != null) {
                    mRender.onSizeChanged(viewWidth, viewHeight);
                    windowHasChanged = false;
                }
                if (mRender != null) {
                    mRender.draw();
                }
            }
        }

        public void surfaceCreated(Surface surface) {
            synchronized (renderManager) {
                mSurface = surface;
                renderManager.notifyAll();
            }

        }

        public void onSizeChanged(int width, int height) {
            synchronized (renderManager) {
                viewWidth = width;
                viewHeight = height;
                windowHasChanged = true;
                renderManager.notifyAll();
            }
        }

        public void onDestory() {
            synchronized (renderManager) {
                shallExit = true;
                hasInit = false;
                windowHasChanged = false;
                viewWidth = -1;
                viewHeight = -1;
                renderManager.notifyAll();
            }

        }
    }

    public class RenderManager {

    }
}
