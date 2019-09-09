package com.bian.myapplication.activity;

import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;

import com.bian.myapplication.R;
import com.bian.myapplication.utils.VideoUtil;

import java.util.List;

import androidx.appcompat.app.AppCompatActivity;

public class RecordVideoActivity extends AppCompatActivity implements View.OnClickListener, Camera.PreviewCallback {
    private SurfaceView mSurfaceView;
    private Camera mCamera;
    private Button recordButton;
    private Button stopRecordButton;
    private List<Camera.Size> cameraLinkedList;
    private AudioRecord mAudioRecord;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_record_video);
        initView();
        initRecorder();
    }

    private void initRecorder() {
        mCamera = Camera.open();
        Camera.Parameters parameters = mCamera.getParameters();
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
        parameters.setPreviewFormat(ImageFormat.YUV_420_888);
        parameters.getSupportedPreviewSizes();
        mCamera.setParameters(parameters);
        mCamera.setPreviewCallback(this);
        setCameraPreviewParam(parameters);


    }

    private void initView() {
        mSurfaceView = findViewById(R.id.sfv_main);
        recordButton = findViewById(R.id.bt_record_video);
        stopRecordButton = findViewById(R.id.bt_stop_record_video);
        recordButton.setOnClickListener(this);
        stopRecordButton.setOnClickListener(this);

    }

    private void setCameraPreviewParam(Camera.Parameters parameters) {
        cameraLinkedList.addAll(parameters.getSupportedPreviewSizes());
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.bt_record_video:
                beginPreview();
                break;
            case R.id.bt_stop_record_video:
                mCamera.stopPreview();
                break;
        }
    }

    private void beginPreview() {
        int dstHight = mSurfaceView.getHeight();
        int dstWidth = mSurfaceView.getWidth();
        for (Camera.Size size : cameraLinkedList) {
        }
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {

    }
}
