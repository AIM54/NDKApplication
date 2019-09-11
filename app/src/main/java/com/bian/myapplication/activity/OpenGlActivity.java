package com.bian.myapplication.activity;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;

import com.bian.myapplication.R;
import com.bian.myapplication.utils.CommonLog;
import com.bian.myapplication.view.PictureGLSurfaceView;

public class OpenGlActivity extends AppCompatActivity {
    private PictureGLSurfaceView pictureGLSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_open_gl);
        pictureGLSurfaceView = findViewById(R.id.glv_main);
        CommonLog.i("currentActivity can use opengl 3.0?:" + detectOpenGLES30());
    }

    private boolean detectOpenGLES30() {
        ActivityManager am = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
        ConfigurationInfo info = am.getDeviceConfigurationInfo();
        return (info.reqGlEsVersion >= 0x30000);
    }

    @Override
    protected void onPause() {
        super.onPause();
        pictureGLSurfaceView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        pictureGLSurfaceView.onResume();
    }
}
