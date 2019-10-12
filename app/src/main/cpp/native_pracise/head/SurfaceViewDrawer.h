//
// Created by Administrator on 2019/10/8.
//

#ifndef NDKAPPLICATION_SURFACEVIEWDRAWER_H
#define NDKAPPLICATION_SURFACEVIEWDRAWER_H

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
#include <android/window.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/asset_manager.h>


class SurfaceViewDrawer {
private:
    ANativeWindow *aNativeWindow = nullptr;
    char *vertShader;
    char *fragmentShader;
    EGLint majorVersion;
    EGLint minorVersion;
    EGLSurface eglWindow;

public:

    SurfaceViewDrawer(JNIEnv *jniEnv, jobject surface);

    virtual ~SurfaceViewDrawer();

    void init();

    void resize(int width, int height);

    void step();

};

char *getStringFromAssert(AAssetManager *aAssetManager, char *path);

#endif //NDKAPPLICATION_SURFACEVIEWDRAWER_H
