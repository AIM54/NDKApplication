//
// Created by Administrator on 2019/10/8.
//

#include <cstring>
#include <android/asset_manager_jni.h>
#include <malloc.h>
#include "SurfaceViewDrawer.h"
#include "AndroidLog.h"
#include <math.h>
#include "jpeglib.h"
#include "esUtil.h"
static GLuint VERTEX_POS_SIZE = 3;
static GLuint VERTEX_COLOR_SIZE = 4;

static GLuint VERTEX_POS_INDEX = 0;

static GLuint VERTEX_COLOR_INDEX = 1;


extern "C" {
#include "AssetReader.h"
}
GLuint dataBuffer[2];

GLuint vertexArray[1];

SurfaceViewDrawer::SurfaceViewDrawer(JNIEnv *jniEnv, jobject surface, jobject assert)
        : mProgramObject(0) {
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
    char *fragmentShaderString = readStringFromAssert(assetManager, "second_shader.glsl");
    char *veticalShaderString = readStringFromAssert(assetManager, "second_v.glsl");
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, veticalShaderString);
    if (!vertexShader) {
        ALOGI("failed to create vertexShader");
        return;
    }
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderString);
    if (!fragmentShader) {
        ALOGI("failed to create fragmentShader");
        return;
    }
    GLuint programObject = glCreateProgram();
    if (!programObject) {
        ALOGI("failed to create programObject");
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
    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    mProgramObject = programObject;
    ALOGI("after init gl");
    delete[]fragmentShaderString;
    delete[]veticalShaderString;

}


void SurfaceViewDrawer::resize(int width, int height) {
    this->viewWidth = width;
    this->viewHeight = height;
}

void SurfaceViewDrawer::step() {
    GLfloat vColorAndPostion[] = {0.0f, 0.5f, 0.0f,
                                  0.0f, 1.0f, 0.0f, 1.0f,
                                  -0.5f, -0.5f, 0.0f,
                                  1.0f, 1.0f, 0.0f, 1.0f,
                                  0.5f, -0.5f, 0.0f,
                                  0.0f, 1.0f, 1.0f, 1.0f,
    };

    GLshort indices[3] = {0, 1, 2};
    glUseProgram(mProgramObject);
    initVAO(vColorAndPostion, 3, sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE), 3,
            indices);
    // Set the viewport
    glViewport(0, 0, viewWidth, viewHeight);
    //clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    //use the programObject;
    drawVAO();
    eglSwapBuffers(disPlay, eglWindow);
}

void drawVAO() {
    glBindVertexArray(vertexArray[0]);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

void initVAO(GLfloat *dataArray, GLint verticalNumbs, GLint stride, GLint indexSize,
             GLshort *indexa) {

    glGenBuffers(2, dataBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, dataBuffer[0]);
    glBufferData(GL_ARRAY_BUFFER, verticalNumbs * stride, dataArray, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dataBuffer[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort) * indexSize, indexa, GL_STATIC_DRAW);
    glGenVertexArrays(1, vertexArray);
    glBindVertexArray(vertexArray[0]);
    glBindBuffer(GL_ARRAY_BUFFER, dataBuffer[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dataBuffer[1]);

    glEnableVertexAttribArray(VERTEX_POS_INDEX);
    glVertexAttribPointer(VERTEX_POS_INDEX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, stride,
                          (const void *) 0);

    glEnableVertexAttribArray(VERTEX_COLOR_INDEX);
    glVertexAttribPointer(VERTEX_COLOR_INDEX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, stride,
                          (const void *) (sizeof(GLfloat) * VERTEX_POS_SIZE));
    glBindVertexArray(0);
}


void drawWithVBO(GLfloat *dataArray, GLint verticalNumbs, GLint stride, GLint indexSize,
                 GLshort *indexa) {
    GLint offset = 0;
    if (dataBuffer[0] == 0 && dataBuffer[1] == 0) {
        glGenBuffers(2, dataBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, dataBuffer[0]);
        glBufferData(GL_ARRAY_BUFFER, stride * verticalNumbs, dataArray, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dataBuffer[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort) * indexSize, indexa, GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, dataBuffer[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dataBuffer[1]);

    glEnableVertexAttribArray(VERTEX_POS_INDEX);
    glVertexAttribPointer(VERTEX_POS_INDEX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, stride,
                          (const void *) offset);

    glEnableVertexAttribArray(VERTEX_COLOR_INDEX);
    offset += VERTEX_POS_SIZE * sizeof(GLfloat);
    glVertexAttribPointer(VERTEX_COLOR_INDEX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, stride,
                          (const void *) offset);

    glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(VERTEX_COLOR_INDEX);
    glDisableVertexAttribArray(VERTEX_POS_INDEX);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void drawWithoutVBOs(GLfloat *dataArray, GLint stride, GLint numberIndex, GLshort *indexs) {
    GLfloat *vtxBuf = dataArray;
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, vtxBuf);
    vtxBuf += 3;
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, vtxBuf);
    glDrawElements(GL_TRIANGLES, numberIndex, GL_UNSIGNED_SHORT, indexs);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
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
    if (mProgramObject) {
        glDeleteProgram(mProgramObject);
        mProgramObject = 0;
    }
    glDeleteBuffers(2, dataBuffer);
    glDeleteVertexArrays(1, vertexArray);
}


