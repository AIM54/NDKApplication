//
// Created by Administrator on 2019/12/6.
//

#include <math.h>
#include <time.h>
#include "InstancingDrawer.h"
#include "esUtil.h"

extern "C" {
#include "TimeUtil.h"
}

InstancingDrawer::InstancingDrawer(JNIEnv *jniEnv, const _jobject *surface,
                                   const _jobject *pJobject) : BaseOpenGlDrawer(jniEnv,
                                                                                const_cast<jobject>(surface),
                                                                                const_cast<jobject>(pJobject)) {

}

int InstancingDrawer::init() {
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
    numIndices = esGenCube(0.1f, &positions,
                           NULL, NULL, &indices);

    // Index buffer object
    glGenBuffers(1, &indicesIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numIndices, indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    free(indices);

    // Position VBO for cube model
    glGenBuffers(1, &positionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat) * 3, positions, GL_STATIC_DRAW);
    free(positions);

    // Random color for each instance
    {
        GLubyte colors[NUM_INSTANCES][4];
        int instance;

        srandom(0);

        for (instance = 0; instance < NUM_INSTANCES; instance++) {
            colors[instance][0] = random() % 255;
            colors[instance][1] = random() % 255;
            colors[instance][2] = random() % 255;
            colors[instance][3] = 0;
        }

        glGenBuffers(1, &colorVBO);
        glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        glBufferData(GL_ARRAY_BUFFER, NUM_INSTANCES * 4, colors, GL_STATIC_DRAW);
    }

    // Allocate storage to store MVP per instance
    {
        int instance;

        // Random angle for each instance, compute the MVP later
        for (instance = 0; instance < NUM_INSTANCES; instance++) {
            angle[instance] = (float) (random() % 32768) / 32767.0f * 360.0f;
        }

        glGenBuffers(1, &mvpVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mvpVBO);
        glBufferData(GL_ARRAY_BUFFER, NUM_INSTANCES * sizeof(ESMatrix), NULL, GL_DYNAMIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    ALOGI("InstancingDrawer init()");
    return GL_TRUE;
}

void InstancingDrawer::step() {
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
    glVertexAttribPointer(COLOR_LOC, 4, GL_UNSIGNED_BYTE,
                          GL_TRUE, 4 * sizeof(GLubyte), (const void *) NULL);
    glEnableVertexAttribArray(COLOR_LOC);
    glVertexAttribDivisor(COLOR_LOC, 1); // One color per instance


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
    // Draw the cubes
    update();

    glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, (const void *) NULL,
                            NUM_INSTANCES);

    eglSwapBuffers(disPlay, eglWindow);
}


void InstancingDrawer::update() {

    ESMatrix *matrixBuf;
    ESMatrix perspective;
    float aspect;
    int instance = 0;
    int numRows;
    int numColumns;


    // Compute the window aspect ratio
    aspect = (GLfloat) viewWidth / (GLfloat) viewHeight;

    // Generate a perspective matrix with a 60 degree FOV
    esMatrixLoadIdentity(&perspective);
    esPerspective(&perspective, 60.0f, aspect, 1.0f, 20.0f);

    glBindBuffer(GL_ARRAY_BUFFER, mvpVBO);
    matrixBuf = (ESMatrix *) glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(ESMatrix) * NUM_INSTANCES,
                                              GL_MAP_WRITE_BIT);

    // Compute a per-instance MVP that translates and rotates each instance differnetly
    numRows = (int) sqrtf(NUM_INSTANCES);
    numColumns = numRows;
    float curTime = getCurrentTime();
    float deltaTime = (curTime - lastTime);
    lastTime = curTime;
    ALOGI("deltaTime:%f", deltaTime);
    for (instance = 0; instance < NUM_INSTANCES; instance++) {
        ESMatrix modelview;
        float translateX = ((float) (instance % numRows) / (float) numRows) * 2.0f - 1.0f;
        float translateY = ((float) (instance / numColumns) / (float) numColumns) * 2.0f - 1.0f;

        // Generate a model view matrix to rotate/translate the cube
        esMatrixLoadIdentity(&modelview);

        // Per-instance translation
        esTranslate(&modelview, translateX, translateY, -2.0f);

        // Compute a rotation angle based on time to rotate the cube
        angle[instance] += (deltaTime * 40.0f);

        if (angle[instance] >= 360.0f) {
            angle[instance] -= 360.0f;
        }

        // Rotate the cube
        esRotate(&modelview, angle[instance], 1.0, 0.0, 1.0);

        // Compute the final MVP by multiplying the
        // modevleiw and perspective matrices together
        esMatrixMultiply(&matrixBuf[instance], &modelview, &perspective);
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
}

InstancingDrawer::~InstancingDrawer() {


    glDeleteBuffers(1, &positionVBO);
    glDeleteBuffers(1, &colorVBO);
    glDeleteBuffers(1, &mvpVBO);
    glDeleteBuffers(1, &indicesIBO);

}
