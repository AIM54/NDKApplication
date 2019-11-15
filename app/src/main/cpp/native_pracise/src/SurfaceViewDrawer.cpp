//
// Created by Administrator on 2019/10/8.
//

#include <cstring>
#include <android/asset_manager_jni.h>
#include <malloc.h>
#include "SurfaceViewDrawer.h"
#include "AndroidLog.h"

extern "C" {
#include "AssetReader.h"
}

SurfaceViewDrawer::SurfaceViewDrawer(JNIEnv *jniEnv, jobject surface, jobject assert) {
    this->aNativeWindow = ANativeWindow_fromSurface(jniEnv, surface);
    this->assetManager = AAssetManager_fromJava(jniEnv, assert);
}

void SurfaceViewDrawer::init() {
    disPlay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
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
        return;
    }
    EGLint eglContextConfigs[]{EGL_CONTEXT_CLIENT_VERSION, 3,
                               EGL_NONE};

    eglContext = eglCreateContext(disPlay, eglConfig, EGL_NO_CONTEXT, eglContextConfigs);
    if (eglContext == EGL_NO_CONTEXT) {
        ALOGI("failed to init Context");
        return;
    }
    if (!eglMakeCurrent(disPlay, eglWindow, eglWindow, eglContext)) {
        ALOGI("failed to make sure");
        return;
    }
    readGlsl();
}

void SurfaceViewDrawer::readGlsl() {
    char *fragmentShaderString = readStringFromAssert(assetManager, "first_shader.glsl");
    char *veticalShaderString = readStringFromAssert(assetManager, "first_v.glsl");
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, veticalShaderString);
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderString);
    GLuint programObject = glCreateProgram();
    if (!programObject) {
        return;
    }
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    glLinkProgram(programObject);
    GLint linked;
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint inforLen = 0;
        glGetShaderiv(programObject, GL_INFO_LOG_LENGTH, &inforLen);
        if (inforLen > 0) {
            char *logInfor = static_cast<char *>(malloc(inforLen * sizeof(char)));
            glGetShaderInfoLog(programObject, inforLen, nullptr, logInfor);
            ALOGE("link the opengl failed:%s", logInfor);
            free(logInfor);
        }
        glDeleteProgram(programObject);
        return;
    }
    glClearColor(1.0f, 1.0f, 0.0f, 0.0f);
    mProgramObject = programObject;
    ALOGI("after init gl");
    delete[]fragmentShaderString;
    delete[]veticalShaderString;

}


void SurfaceViewDrawer::resize(int width, int height) {
    this->viewWidth = width;
    this->viewHeight = height;
    ALOGI("viewWidth:%d||viewHeight:%d", viewWidth, viewHeight);
}

void SurfaceViewDrawer::step() {
    GLfloat vVertices[] = {0.0f, 0.5f, 0.0f,
                           -0.5f, -0.5f, 0.0f,
                           0.5f, -0.5f, 0.0f
    };
    // Set the viewport
    glViewport(0, 0, viewWidth, viewHeight);
    //clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    //use the programObject;
    glUseProgram(mProgramObject);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    eglSwapBuffers(disPlay,eglWindow);
}

SurfaceViewDrawer::~SurfaceViewDrawer() {
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


