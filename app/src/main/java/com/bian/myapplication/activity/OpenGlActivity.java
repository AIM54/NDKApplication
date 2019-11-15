package com.bian.myapplication.activity;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;

import com.bian.myapplication.R;
import com.bian.myapplication.fragment.FirstFragment;
import com.bian.myapplication.utils.CommonLog;
import com.bian.myapplication.view.PictureGLSurfaceView;

public class OpenGlActivity extends AppCompatActivity {
    public static final String ARG_POSTION = "ARG_POSTION";
    private Fragment fragment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_open_gl);
        createFragment();
        commitFragment();
    }

    private void createFragment() {
        int postion = getIntent().getIntExtra(ARG_POSTION, 0);
        switch (postion) {
            case 0:
                fragment = new FirstFragment();
                break;
            default:
                fragment = new FirstFragment();
                break;
        }
    }

    private void commitFragment() {
        getSupportFragmentManager().beginTransaction().add(R.id.fl_container, fragment, fragment.getClass().getSimpleName())
                .commit();
    }
}
