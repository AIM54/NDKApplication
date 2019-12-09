package com.bian.myapplication.fragment;


import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import com.bian.myapplication.R;
import com.bian.myapplication.utils.SurfaceDrawer;
import com.bian.myapplication.view.MySurfaceView;

/**
 *
 */
public class CubeOneFragment extends Fragment{
    private static final String ARG_PARAM1 = "param1";




    private MySurfaceView mainSv;
    private SurfaceDrawer surfaceDrawer;
    private int mParam1;

    public static CubeOneFragment newInstance(int param1) {
        CubeOneFragment fragment = new CubeOneFragment();
        Bundle args = new Bundle();
        args.putInt(ARG_PARAM1, param1);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            mParam1 = getArguments().getInt(ARG_PARAM1);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_new_cube, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        mainSv = view.findViewById(R.id.sfv_main);
        mainSv.setRender(new MySurfaceView.MyRender() {
            @Override
            public void init(Surface surface) {
                if (surfaceDrawer == null) {
                    surfaceDrawer = new SurfaceDrawer(surface, getContext().getAssets(), mParam1);
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
