//
// Created by Administrator on 2019/11/23.
//

#include "BitmapDrawer.h"
#include "AndroidLog.h"

extern "C" {
#include "AssetReader.h"
}

BitmapDrawer::BitmapDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *pJobject,
                           jobject bitmap)
        : BaseOpenGlDrawer(jniEnv, const_cast<jobject>(surface), const_cast<jobject>(pJobject)) {
    mEnv = jniEnv;
    mBitMap = bitmap;
    ALOGE("BitmapDrawer created");
}

int BitmapDrawer::init() {
    if (!BaseOpenGlDrawer::init()) {
        ALOGI("call super method failed");
        return -1;
    }
    initProgram("first_v.glsl", "first_shader.glsl");
    if (!mProgramObject) {
        return -1;
    }
    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
//    glGenTextures(1, bitmapTexture);
//    glBindTexture(GL_TEXTURE_2D, bitmapTexture[0]);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glBindTexture(GL_TEXTURE_2D, GL_NONE);
//    sTexture = glGetUniformLocation(mProgramObject, "s_texture");
    return 0;
}

void BitmapDrawer::step() {
    if (!mProgramObject) {
        ALOGE("program failed to create!");
        return;
    }
    GLfloat vVertices[] = {0.0f, 0.5f, 0.0f,
                           -0.5f, -0.5f, 0.0f,
                           0.5f, -0.5f, 0.0f
    };
    GLfloat aColor[4] = {1.0, 0.0, 0.0, 1.0};
    // Set the viewport
    glViewport(0, 0, viewWidth, viewHeight);
    ALOGI("viewWidth:%d|viewHeight:%d",viewWidth,viewHeight);
    //clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    //use the programObject;
    glUseProgram(mProgramObject);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);
    glVertexAttrib4fv(1, aColor);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);
    eglSwapBuffers(disPlay, eglWindow);
    ALOGE("step");
}

BitmapDrawer::~BitmapDrawer() {
    ALOGE("delete bitmap drawer");
}


