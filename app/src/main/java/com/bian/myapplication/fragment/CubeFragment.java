package com.bian.myapplication.fragment;


import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import com.bian.myapplication.R;
import com.bian.myapplication.utils.CommonLog;
import com.bian.myapplication.utils.SurfaceDrawer;
import com.bian.myapplication.view.MySurfaceView;

/**
 * A simple {@link Fragment} subclass.
 */
public class CubeFragment extends Fragment {
    private MySurfaceView mainSv;
    private SurfaceDrawer surfaceDrawer;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_cube, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        mainSv = view.findViewById(R.id.sfv_main);
        mainSv.setRender(new MySurfaceView.MyRender() {
            @Override
            public void init(Surface surface) {
                if (surfaceDrawer == null) {
                    surfaceDrawer = new SurfaceDrawer(surface, getContext().getAssets(), 3);
                }
            }

            @Override
            public void onSizeChanged(int width, int height) {
                surfaceDrawer.resizeSurfaceView(width, height);
            }

            @Override
            public void draw() {
                surfaceDrawer.stepSurfaceView();
            }

            @Override
            public void destroy() {
                surfaceDrawer.destroyView();
                surfaceDrawer = null;
            }
        });
    }

}
