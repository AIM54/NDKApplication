//
// Created by Administrator on 2019/11/30.
//

#include "CubeDrawer.h"
#include "esUtil.h"
#include <time.h>
#include <InstancingDrawer.h>
#include <math.h>

extern "C" {
#include "TimeUtil.h"
}
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
        : BaseOpenGlDrawer(jniEnv, const_cast<jobject>(surface), const_cast<jobject>(assert)) {
}

int CubeDrawer::init() {
    if (!BaseOpenGlDrawer::init()) {
        return -1;
    }
    initProgram("first_cube_v.glsl", "first_cube_shader.glsl");
    if (!mProgramObject) {
        return -1;
    }
    lastTime = getCurrentTime();
    GLfloat *positions;
    GLuint *indices;
    // Generate the vertex data
    numIndices = esGenCube(1.0f, &positions,
                           NULL, NULL, &indices);

    // Index buffer object
    glGenBuffers(1, &indicesIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numIndices, indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
    free(indices);

    // Position VBO for cube model
    glGenBuffers(1, &positionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat) * 3, positions, GL_STATIC_DRAW);
    free(positions);

    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColor), cubeColor, GL_STATIC_DRAW);

    glGenBuffers(1, &mvpVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mvpVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ESMatrix), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    ALOGI("InstancingDrawer init()");
    return GL_TRUE;
}

void CubeDrawer::step() {
    // Draw the cubes
    update();
    // Set the viewport
    glViewport(0, 0, viewWidth, viewHeight);

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the program object
    glUseProgram(mProgramObject);

    // Load the vertex position
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glVertexAttribPointer(POSITION_LOC, 3, GL_FLOAT,
                          GL_FALSE, 3 * sizeof(GLfloat), (const void *) NULL);
    glEnableVertexAttribArray(POSITION_LOC);

    // Load the instance color buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glVertexAttribPointer(COLOR_LOC, 4, GL_FLOAT,
                          GL_TRUE, 4 * sizeof(GL_FLOAT), (const void *) NULL);
    glEnableVertexAttribArray(COLOR_LOC);


    // Load the instance MVP buffer
    glBindBuffer(GL_ARRAY_BUFFER, mvpVBO);

    // Load each matrix row of the MVP.  Each row gets an increasing attribute location.
    glVertexAttribPointer(MVP_LOC + 0, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix),
                          (const void *) NULL);
    glVertexAttribPointer(MVP_LOC + 1, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix),
                          (const void *) (sizeof(GLfloat) * 4));
    glVertexAttribPointer(MVP_LOC + 2, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix),
                          (const void *) (sizeof(GLfloat) * 8));
    glVertexAttribPointer(MVP_LOC + 3, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix),
                          (const void *) (sizeof(GLfloat) * 12));
    glEnableVertexAttribArray(MVP_LOC + 0);
    glEnableVertexAttribArray(MVP_LOC + 1);
    glEnableVertexAttribArray(MVP_LOC + 2);
    glEnableVertexAttribArray(MVP_LOC + 3);

    // One MVP per instance
    glVertexAttribDivisor(MVP_LOC + 0, 1);
    glVertexAttribDivisor(MVP_LOC + 1, 1);
    glVertexAttribDivisor(MVP_LOC + 2, 1);
    glVertexAttribDivisor(MVP_LOC + 3, 1);

    // Bind the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesIBO);

    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, (const void *) NULL);

    eglSwapBuffers(disPlay, eglWindow);
}

void CubeDrawer::update() {
    ESMatrix *matrixBuf;
    ESMatrix perspective;
    float aspect;
    // Compute the window aspect ratio
    aspect = (GLfloat) viewWidth / (GLfloat) viewHeight;

    // Generate a perspective matrix with a 60 degree FOV
    esMatrixLoadIdentity(&perspective);
    esPerspective(&perspective, 60.0f, aspect, 1.0f, 20.0f);

    glBindBuffer(GL_ARRAY_BUFFER, mvpVBO);
    matrixBuf = (ESMatrix *) glMapBufferRange(GL_ARRAY_BUFFER, 0,
                                              sizeof(ESMatrix) ,
                                              GL_MAP_WRITE_BIT);

    // Compute a per-instance MVP that translates and rotates each instance differnetly
    float curTime = getCurrentTime();
    float deltaTime = (curTime - lastTime);
    lastTime = curTime;
    ALOGI("deltaTime:%f", deltaTime);
    ESMatrix modelview;
    esMatrixLoadIdentity(&modelview);
    // Per-instance translation
    esTranslate(&modelview, 0.0, 0.0, -2.0f);
    angle += (deltaTime * 40.0f);
    esRotate(&modelview, angle, 1.0, 0.0, 1.0);

    // Compute the final MVP by multiplying the
    // modevleiw and perspective matrices together
    esMatrixMultiply(&matrixBuf[0], &modelview, &perspective);
    glEnable(GL_DEPTH_TEST);
    glUnmapBuffer(GL_ARRAY_BUFFER);

}

CubeDrawer::~CubeDrawer() {

    glDeleteProgram(mProgramObject);
}



