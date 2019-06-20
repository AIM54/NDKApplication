package com.bian.myapplication.video;

import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceView;

import com.bian.myapplication.R;

import androidx.appcompat.app.AppCompatActivity;

public class VideoPlayActivity extends AppCompatActivity {
    public static final String ARG_VIDEO_PATH = "ARG_VIDEO_PATH";
    private SurfaceView mSurfaceView;
    private String stringVideoPath;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_play);
        mSurfaceView = findViewById(R.id.sfv_main);
        beginPlay();
    }

    private void beginPlay() {
        stringVideoPath = getIntent().getStringExtra(ARG_VIDEO_PATH);

    }
}
