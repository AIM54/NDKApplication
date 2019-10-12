//
// Created by Administrator on 2019/10/8.
//

#include <cstring>
#include "SurfaceViewDrawer.h"

#include "AndroidLog.h"


SurfaceViewDrawer::SurfaceViewDrawer(JNIEnv *jniEnv, jobject surface) {
    aNativeWindow = ANativeWindow_fromSurface(jniEnv, surface);
    if (!aNativeWindow) {
        ALOGE("failed to create native window");
    }
}

void SurfaceViewDrawer::init() {
    EGLDisplay disPlay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (disPlay == EGL_NO_DISPLAY) {
        ALOGI("current device not support the opengl draw");
        return;
    }
    int openGLStatus = eglInitialize(disPlay, &majorVersion, &minorVersion);
    if (!openGLStatus) {
        ALOGI("current device failed to init opengl");
        return;
    }

    EGLint attributes[]{
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_NONE
    };
    EGLConfig eglConfig;
    EGLint configNumber;
    if (!eglChooseConfig(disPlay, attributes, &eglConfig, 1, &configNumber)) {
        ALOGI("faild to create render Environment");
        return;
    }
    EGLint configs[]{
            EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
            EGL_NONE
    };
    eglWindow = eglCreateWindowSurface(disPlay, eglConfig, aNativeWindow, configs);
    if (eglWindow == EGL_NO_SURFACE) {
        ALOGI("failed to init Surface");
    }

    EGLint eglContextConfigs[]{EGL_CONTEXT_CLIENT_VERSION, 3,
                               EGL_NONE};

    EGLContext eglContext = eglCreateContext(disPlay, eglConfig, EGL_NO_CONTEXT, eglContextConfigs);
    if (eglContext == EGL_NO_CONTEXT) {
        ALOGI("failed to init Context");
        return;
    }
    if (!eglMakeCurrent(disPlay, eglWindow, eglWindow, eglContext)) {
        ALOGI("failed to make sure");
        return;
    }
    ALOGI("create opengl success");
    eglSwapBuffers(disPlay, eglWindow);
}

void SurfaceViewDrawer::resize(int width, int height) {

}

void SurfaceViewDrawer::step() {

}

SurfaceViewDrawer::~SurfaceViewDrawer() {

}


