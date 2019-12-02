package com.bian.myapplication.activity;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.bian.myapplication.R;
import com.bian.myapplication.image.ImageListActivity;
import com.bian.myapplication.utils.AppConstant;
import com.bian.myapplication.utils.CommonLog;
import com.bian.myapplication.utils.SurfaceDrawer;

public class ImageActivity extends AppCompatActivity implements SurfaceHolder.Callback {
    SurfaceView surfaceView;
    public static final int REQUEST_CODE = 110;
    private String mPicturePath;
    private SurfaceDrawer surfaceDrawer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_image);
        initView();
    }

    private void initView() {
        surfaceView = findViewById(R.id.sfv_main);
        surfaceView.getHolder().addCallback(this);
        findViewById(R.id.bt_select_pic).setOnClickListener(v -> {
            Intent imageIt = new Intent(this, ImageListActivity.class);
            imageIt.setAction(AppConstant.SELECT_PICTURE);
            startActivityForResult(imageIt, REQUEST_CODE);
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (data == null) {
            return;
        }
        if (requestCode == REQUEST_CODE && resultCode == RESULT_OK) {
            mPicturePath = data.getStringExtra(AppConstant.ARG_VIDEO_PATH);
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        CommonLog.i("surfaceCreated");
        if (!TextUtils.isEmpty(mPicturePath)) {
            if (surfaceDrawer == null) {
                Bitmap trueBitmap = BitmapFactory.decodeFile(mPicturePath);
                surfaceDrawer = new SurfaceDrawer(holder.getSurface(), getAssets(), 1, trueBitmap);
            }

        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        if (surfaceDrawer != null) {
            surfaceDrawer.resizeSurfaceView(width, height);
        }
        surfaceDrawer.stepSurfaceView();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        CommonLog.i("surfaceDestroyed");
        if (surfaceDrawer != null) {
            surfaceDrawer.destroyView();
            surfaceDrawer = null;
        }
    }
}
