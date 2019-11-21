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
    ANativeWindow *aNativeWindow;
    AAssetManager *assetManager;
    EGLint majorVersion;
    EGLint minorVersion;
    EGLSurface eglWindow;
    EGLDisplay disPlay;
    EGLContext eglContext;
    int viewWidth;
    int viewHeight;

protected:
    void readGlsl();

public:

    SurfaceViewDrawer(JNIEnv *jniEnv, jobject surface, jobject pJobject);

    virtual ~SurfaceViewDrawer();

    void init();


    void resize(int width, int height);

    void step();


    GLuint mProgramObject;
};

void drawWithoutVBOs(GLfloat *dataArray, GLint stride, GLint numberIndex, GLshort *indexs);

void drawWithVBO(GLfloat *dataArray, GLint verticalNumbs, GLint stride, GLint indexSize,
                 GLshort *indexa);

void initVAO(GLfloat *dataArray, GLint verticalNumbs, GLint stride, GLint indexSize,
             GLshort *indexa);

void drawVAO();


void drawWithVAO();

#endif //NDKAPPLICATION_SURFACEVIEWDRAWER_H
