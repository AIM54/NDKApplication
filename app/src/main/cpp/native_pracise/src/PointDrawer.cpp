//
// Created by Administrator on 2019/11/21.
//

extern "C" {
#include "AssetReader.h"
}

#include "PointDrawer.h"
#include "AndroidLog.h"
#include <math.h>
#include <esUtil.h>

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
    return 0;
}

void PointDrawer::step() {
    GLfloat vVertices[] = {0.0f, 0.5f, 0.0f,
                           -0.5f, -0.5f, 0.0f,
                           0.5f, -0.5f, 0.0f
    };
    GLfloat aColor[4] = {1.0, 1.0, 0.0, 1.0};
    // Set the viewport
    glViewport(0, 0, viewWidth, viewHeight);
    //clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    //use the programObject;
    glUseProgram(mProgramObject);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);
    glVertexAttrib4fv(1, aColor);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    eglSwapBuffers(disPlay, eglWindow);

}

