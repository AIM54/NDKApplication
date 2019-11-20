//
// Created by Administrator on 2019/11/17.
//

#ifndef NDKAPPLICATION_BASEOPENGLDRAWER_H
#define NDKAPPLICATION_BASEOPENGLDRAWER_H

#include <jni.h>
#include "AndroidLog.h"
#include <malloc.h>
#include <cstring>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

class BaseOpenGlDrawer {
protected:
    ANativeWindow *aNativeWindow;
    AAssetManager *assetManager;
    EGLint majorVersion;
    EGLint minorVersion;
    EGLSurface eglWindow;
    EGLDisplay disPlay;
    EGLContext eglContext;
    int viewWidth;
    int viewHeight;

public:
    BaseOpenGlDrawer(JNIEnv *jniEnv, jobject surface, jobject pJobject);
    virtual int init();

    virtual ~BaseOpenGlDrawer();

};


#endif //NDKAPPLICATION_BASEOPENGLDRAWER_H
