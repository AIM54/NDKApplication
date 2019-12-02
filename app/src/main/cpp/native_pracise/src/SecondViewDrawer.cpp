//
// Created by Administrator on 2019/11/20.
//

#include "SecondViewDrawer.h"
#include "AndroidLog.h"

extern "C" {
#include "AssetReader.h"
}

SecondViewDrawer::SecondViewDrawer(JNIEnv *jniEnv, const _jobject *surface,
                                   const _jobject *pJobject) : BaseOpenGlDrawer(jniEnv,
                                                                                const_cast<jobject>(surface),
                                                                                const_cast<jobject>(pJobject)) {

}

int SecondViewDrawer::init() {
    if (!BaseOpenGlDrawer::init()) {
        return 0;
    }
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    char *vertString = readStringFromAssert(assetManager, "second_v.glsl");
    char *fragmentString = readStringFromAssert(assetManager, "second_shader.glsl");
    createProgram(vertString, fragmentString);
    GLfloat vColorAndPostion[] = {-0.8f, 0.8f, 0.0f,
                                  0.0f, 1.0f, 0.0f, 1.0f,

                                  -0.8f, -0.8f, 0.0f,
                                  0.0f, 1.0f, 0.0f, 1.0f,

                                  0.8f, -0.8f, 0.0f,
                                  1.0f, 0.0f, 1.0f, 1.0f,

                                  0.8f, 0.8f, 0.0f,
                                  1.0f, 0.0f, 1.0f, 1.0f,
    };

    GLshort indices[6] = {0, 1, 2, 2, 0, 3};
    //use the programObject;
    glUseProgram(mProgramObject);
    initVAO(vColorAndPostion, indices, sizeof(GLfloat) * (VERTEX_COLOR_SIZE + VERTEX_POS_SIZE), 6,
            6);
    return 1;
}

void SecondViewDrawer::step() {
    glViewport(0, 0, viewWidth, viewHeight);
    //clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    drawElement();
    eglSwapBuffers(disPlay, eglWindow);
}


void SecondViewDrawer::initVAO(GLfloat *elementArray, GLshort *indexArray, int stride,
                               int elementSize, int indexSize) {
    glGenBuffers(2, dataBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, dataBuffer[0]);
    glBufferData(GL_ARRAY_BUFFER, stride * elementSize, elementArray, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dataBuffer[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * sizeof(GLshort), indexArray, GL_STATIC_DRAW);

    glGenVertexArrays(1, vertexArray);
    glBindVertexArray(vertexArray[0]);

    glBindBuffer(GL_ARRAY_BUFFER, dataBuffer[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dataBuffer[1]);
    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, stride,
                          (const void *) 0);

    glEnableVertexAttribArray(VERTEX_COLOR_INDX);
    glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, stride,
                          (const void *) (VERTEX_POS_SIZE *
                                          sizeof(GLfloat)));
    glBindVertexArray(0);
}

void SecondViewDrawer::drawElement() {
    glBindVertexArray(vertexArray[0]);
    glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

SecondViewDrawer::~SecondViewDrawer() {
    glDeleteVertexArrays(1, vertexArray);
    glDeleteBuffers(2, dataBuffer);
}


