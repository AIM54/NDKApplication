package com.bian.myapplication.video;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceView;

import com.bian.myapplication.R;

public class VideoPlayActivity extends AppCompatActivity {
    private SurfaceView mSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_play);
        mSurfaceView = findViewById(R.id.sfv_main);

    }
}
