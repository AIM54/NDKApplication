//
// Created by Administrator on 2019/11/20.
//

#include "SurfaceViewDrawer.h"
#include "SecondViewDrawer.h"

static GLuint VERTEX_POS_SIZE = 3;
static GLuint VERTEX_COLOR_SIZE = 4;

#include "AndroidLog.h"

static GLuint VERTEX_POS_INDEX = 0;

static GLuint VERTEX_COLOR_INDEX = 1;
extern "C" {
#include "AssetReader.h"
}

SecondViewDrawer::SecondViewDrawer(JNIEnv *jniEnv, const _jobject *surface,
                                   const _jobject *pJobject) : BaseOpenGlDrawer(jniEnv,
                                                                                const_cast<jobject>(surface),
                                                                                const_cast<jobject>(pJobject)) {
    ALOGI("init SecondViewDrawer");
}

int SecondViewDrawer::init() {
    if (!BaseOpenGlDrawer::init()) {
        return 0;
    }
    char *vertString = readStringFromAssert(assetManager, "second_v.glsl");
    char *fragmentString = readStringFromAssert(assetManager, "second_shader.glsl");
    createProgram(vertString, fragmentString);
    return 1;
}

void SecondViewDrawer::step() {
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



