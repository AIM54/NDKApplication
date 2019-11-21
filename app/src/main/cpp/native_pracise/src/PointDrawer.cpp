//
// Created by Administrator on 2019/11/21.
//

extern "C" {
#include "AssetReader.h"
}

#include "PointDrawer.h"
#include "AndroidLog.h"

PointDrawer::PointDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *pJobject)
        : BaseOpenGlDrawer(jniEnv, const_cast<jobject>(surface), const_cast<jobject>(pJobject)) {

}

int PointDrawer::init() {
    if (!BaseOpenGlDrawer::init()) {
        return 0;
    }
    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    char *vertString = readStringFromAssert(assetManager, "first_v.glsl");
    char *fragmentString = readStringFromAssert(assetManager, "first_shader.glsl");
    createProgram(vertString, fragmentString);
    GLfloat vVertices[] = {0.5f, 0.5f, 0.0f,
    };
    GLshort indecs[] = {0};
    return 0;
}

void PointDrawer::step() {

    // Set the viewport
    glViewport(0, 0, viewWidth, viewHeight);
    //clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    //use the programObject;
    eglSwapBuffers(disPlay, eglWindow);

}

