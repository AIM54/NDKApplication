package com.bian.myapplication.fragment;


import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;

import com.bian.myapplication.R;
import com.bian.myapplication.utils.SurfaceDrawer;

/**
 * A simple {@link Fragment} subclass.
 */
public class RectFragment extends Fragment implements SurfaceHolder.Callback {

    private SurfaceView mainSv;
    private SurfaceDrawer surfaceDrawer;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        return inflater.inflate(R.layout.fragment_rect, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        mainSv = view.findViewById(R.id.sfv_main);
        mainSv.getHolder().addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        if (surfaceDrawer == null) {
            surfaceDrawer = new SurfaceDrawer(holder.getSurface(), getContext().getAssets(),1);
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        surfaceDrawer.resizeSurfaceView(width, height);
        surfaceDrawer.stepSurfaceView();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        surfaceDrawer.destroyView();
        surfaceDrawer = null;
    }
}
