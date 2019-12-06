package com.bian.myapplication.activity;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;

import com.bian.myapplication.R;
import com.bian.myapplication.fragment.CubeFragment;
import com.bian.myapplication.fragment.FirstFragment;
import com.bian.myapplication.fragment.LineFragment;
import com.bian.myapplication.fragment.PointFragment;
import com.bian.myapplication.fragment.RectFragment;
import com.bian.myapplication.utils.CommonLog;
import com.bian.myapplication.view.PictureGLSurfaceView;

public class OpenGlActivity extends AppCompatActivity {
    public static final String ARG_POSTION = "ARG_POSTION";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_open_gl);
        commitFragment(new RectFragment());
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.gl_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        switch (item.getItemId()) {
            case R.id.rect_test:
                commitFragment(new RectFragment());
                break;
            case R.id.point_test:
                commitFragment(new PointFragment());
                break;
            case R.id.cube_test:
                commitFragment(new CubeFragment());
                break;
        }
        return true;
    }

    private void commitFragment(Fragment fragment) {
        getSupportFragmentManager().beginTransaction().add(R.id.fl_container, fragment, fragment.getClass().getSimpleName())
                .commit();
    }
}
