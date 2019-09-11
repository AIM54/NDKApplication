package com.bian.myapplication.activity;

import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;

import com.bian.myapplication.R;
import com.bian.myapplication.utils.NativePracise;
import com.bian.myapplication.view.PictureGLSurfaceView;

public class OpenGlActivity extends AppCompatActivity {
    private PictureGLSurfaceView pictureGLSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_open_gl);
        pictureGLSurfaceView = findViewById(R.id.glv_main);

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
