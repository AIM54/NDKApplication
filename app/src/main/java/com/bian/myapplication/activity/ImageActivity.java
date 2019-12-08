package com.bian.myapplication.activity;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.text.TextUtils;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.bian.myapplication.R;
import com.bian.myapplication.dialog.SelectOptionDialog;
import com.bian.myapplication.fragment.CommonFragment;
import com.bian.myapplication.image.ImageListActivity;
import com.bian.myapplication.utils.AppConstant;
import com.bian.myapplication.utils.CommonLog;
import com.bian.myapplication.utils.SurfaceDrawer;

public class ImageActivity extends AppCompatActivity {
    public static final int REQUEST_CODE = 110;
    private String mPicturePath;
    private SelectOptionDialog selectOptionDialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_image);
        initView();
    }

    private void initView() {
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
            showSelectDialog();
        }
    }

    private void showSelectDialog() {
        if (selectOptionDialog == null) {
            String[] videoOpertions = getResources().getStringArray(R.array.test_bitmap);
            ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, videoOpertions);
            selectOptionDialog = new SelectOptionDialog(this, (parent, view, position, id) -> {
                selectOptionDialog.dismiss();
                switchFragment(position);
            });
            selectOptionDialog.setOptionAdapter(adapter);
        }
        selectOptionDialog.show();
    }

    private void switchFragment(int position) {
        CommonFragment commonFragment = CommonFragment.newInstance(position, mPicturePath);
        getSupportFragmentManager().beginTransaction().add(R.id.fl_container, commonFragment, commonFragment.getClass().getSimpleName())
                .commit();
    }
}
