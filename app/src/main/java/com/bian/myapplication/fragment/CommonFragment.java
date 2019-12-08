package com.bian.myapplication.fragment;


import android.graphics.Bitmap;
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

public class CommonFragment extends Fragment implements MySurfaceView.MyRender {
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    private int mParam1;
    private String mBitmapPath;

    private MySurfaceView mySurfaceView;
    private Bitmap trueBitmap;

    private SurfaceDrawer surfaceDrawer;

    public static CommonFragment newInstance(int param1, String param2) {
        CommonFragment fragment = new CommonFragment();
        Bundle args = new Bundle();
        args.putInt(ARG_PARAM1, param1);
        args.putString(ARG_PARAM2, param2);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            mParam1 = getArguments().getInt(ARG_PARAM1);
            mBitmapPath = getArguments().getString(ARG_PARAM2);
            trueBitmap = BitmapFactory.decodeFile(mBitmapPath);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_common, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        mySurfaceView = view.findViewById(R.id.msfv_main);
        mySurfaceView.setRender(this);
    }

    @Override
    public void init(Surface surface) {
        if (surfaceDrawer == null) {
            surfaceDrawer = new SurfaceDrawer(surface, getContext().getAssets(), mParam1, trueBitmap);
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
}
