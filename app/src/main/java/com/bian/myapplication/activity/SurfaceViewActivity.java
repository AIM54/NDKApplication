package com.bian.myapplication.activity;

import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.appcompat.app.AppCompatActivity;

import com.bian.myapplication.R;
import com.bian.myapplication.utils.CommonLog;
import com.bian.myapplication.utils.SurfaceDrawer;


public class SurfaceViewActivity extends AppCompatActivity implements SurfaceHolder.Callback {
    private SurfaceView surfaceView;
    private SurfaceDrawer surfaceDrawer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_to_picture);
        initView();
    }

    private void initView() {
        surfaceView = findViewById(R.id.sfv_main);
        surfaceView.getHolder().addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        if (surfaceDrawer == null) {
            surfaceDrawer = new SurfaceDrawer(holder.getSurface());
        }
        CommonLog.i(" surfaceDrawer.initSurfaceView(holder.getSurface())");
        surfaceDrawer.initSurfaceView(holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        surfaceDrawer = null;
    }
}
