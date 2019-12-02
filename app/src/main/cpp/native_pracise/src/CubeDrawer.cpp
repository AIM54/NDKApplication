//
// Created by Administrator on 2019/11/30.
//

#include "CubeDrawer.h"
#include "esUtil.h"

GLfloat cubeColor[] =
        {
                1.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f, 0.0f,

                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,

                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,

                1.0f, 1.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 0.0f,

                0.0f, 1.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 1.0f, 0.0f,

                1.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
        };

CubeDrawer::CubeDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *assert)
        : hasDraw(false),
          BaseOpenGlDrawer(jniEnv, const_cast<jobject>(surface), const_cast<jobject>(assert)) {
}

int CubeDrawer::init() {
    if (BaseOpenGlDrawer::init()) {
        initProgram("first_cube_v.glsl", "first_cube_shader.glsl");
        if (!mProgramObject) {
            return -1;
        }
        indecsSize = esGenCube(1.0f, &verticals, NULL, NULL, &indics);

        glGenBuffers(1, &postionBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, postionBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * VERTEX_POS_SIZE * VERTEX_NUMBER, verticals,
                     GL_STATIC_DRAW);
        free(verticals);


        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColor), cubeColor, GL_STATIC_DRAW);


        glGenBuffers(1, &indicsBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicsBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indecsSize, indics, GL_STATIC_DRAW);
        free(indics);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glGenBuffers(1, &matrixBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, matrixBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ESMatrix), NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    }
    return 1;
}

void CubeDrawer::step() {
    if (hasDraw) {
        update();
    } else {
        draw();
        hasDraw = true;
        update();
    }

}

void CubeDrawer::draw() {
    ALOGI("draw");
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

void CubeDrawer::update() {

}


