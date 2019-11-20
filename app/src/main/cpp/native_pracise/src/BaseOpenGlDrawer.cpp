//
// Created by Administrator on 2019/11/17.
//


#include "BaseOpenGlDrawer.h"

BaseOpenGlDrawer::BaseOpenGlDrawer(JNIEnv *jniEnv, jobject surface, jobject assert) {
    this->aNativeWindow = ANativeWindow_fromSurface(jniEnv, surface);
    this->assetManager = AAssetManager_fromJava(jniEnv, assert);
}
int BaseOpenGlDrawer::init() {
    disPlay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (disPlay == EGL_NO_DISPLAY) {
        ALOGI("current device not support the opengl draw");
        return 0;
    }
    int openGLStatus = eglInitialize(disPlay, &majorVersion, &minorVersion);
    if (!openGLStatus) {
        ALOGI("current device failed to init opengl");
        return 0;
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
        return 0;
    }
    EGLint configs[]{
            EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
            EGL_NONE
    };
    eglWindow = eglCreateWindowSurface(disPlay, eglConfig, aNativeWindow, configs);
    if (eglWindow == EGL_NO_SURFACE) {
        ALOGI("failed to init Surface");
        return 0;
    }
    EGLint eglContextConfigs[]{EGL_CONTEXT_CLIENT_VERSION, 3,
                               EGL_NONE};

    eglContext = eglCreateContext(disPlay, eglConfig, EGL_NO_CONTEXT, eglContextConfigs);
    if (eglContext == EGL_NO_CONTEXT) {
        ALOGI("failed to init Context");
        return 0;
    }
    if (!eglMakeCurrent(disPlay, eglWindow, eglWindow, eglContext)) {
        ALOGI("failed to make sure");
        return 0;
    }
    return 1;
}

BaseOpenGlDrawer::~BaseOpenGlDrawer() {
    if (eglWindow != EGL_NO_SURFACE) {
        eglDestroySurface(disPlay, eglWindow);
    }
    if (disPlay != EGL_NO_DISPLAY) {
        eglMakeCurrent(disPlay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
    if (eglContext != EGL_NO_CONTEXT) {
        eglDestroyContext(disPlay, eglContext);
    }
    if (disPlay != EGL_NO_DISPLAY) {
        eglReleaseThread();
        eglTerminate(disPlay);
    }
    disPlay = EGL_NO_DISPLAY;
    eglContext = EGL_NO_CONTEXT;
    eglWindow = EGL_NO_SURFACE;
}