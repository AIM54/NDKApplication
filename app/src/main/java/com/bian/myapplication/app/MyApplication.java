package com.bian.myapplication.app;

import android.app.Activity;
import android.app.Application;
import android.os.Bundle;

import com.bian.myapplication.utils.CrashHandler;

import java.util.LinkedList;
import java.util.List;

public class MyApplication extends Application implements Application.ActivityLifecycleCallbacks {
    private static List<Activity> activities;

    @Override
    public void onCreate() {
        super.onCreate();
        activities = new LinkedList<>();
        CrashHandler.getInstance().init(this);
        registerActivityLifecycleCallbacks(this);
    }

    public static void finishAllActivity() {
        for (Activity activity : activities) {
            activity.finish();
        }
    }

    @Override
    public void onActivityCreated(Activity activity, Bundle savedInstanceState) {
        activities.add(activity);
    }

    @Override
    public void onActivityStarted(Activity activity) {

    }

    @Override
    public void onActivityResumed(Activity activity) {

    }

    @Override
    public void onActivityPaused(Activity activity) {

    }

    @Override
    public void onActivityStopped(Activity activity) {

    }

    @Override
    public void onActivitySaveInstanceState(Activity activity, Bundle outState) {

    }

    @Override
    public void onActivityDestroyed(Activity activity) {
        activities.remove(activity);
    }
}
