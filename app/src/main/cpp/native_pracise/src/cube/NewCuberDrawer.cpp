//
// Created by Administrator on 2019/12/5.
//

#include "NewCuberDrawer.h"

NewCuberDrawer::NewCuberDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *pJobject)
        : BaseOpenGlDrawer(jniEnv, const_cast<jobject>(surface), const_cast<jobject>(pJobject)) {
    hasDraw = false;
}

int NewCuberDrawer::init() {
    if (BaseOpenGlDrawer::init()) {
        initProgram("first_cube_v.glsl", "first_cube_shader.glsl");
        if (!mProgramObject) {
            return -1;
        }
        indecsSize = esGenCube(1.0f, &verticals, NULL, NULL, &indics);
    }
    return 1;
}

void NewCuberDrawer::step() {
    draw();
    update();
}

void NewCuberDrawer::update() {
    float aspect = (GLfloat) viewWidth / (GLfloat) viewHeight;
    ESMatrix modelMatraix;
    ESMatrix persinMatrix;

    esMatrixLoadIdentity(&persinMatrix);
    esPerspective(&persinMatrix, 60.0f, aspect, 1.0f, 20.0f);

    esMatrixLoadIdentity(&modelMatraix);
    esTranslate(&modelMatraix, 0.0, 0.0, -2.0);
    esRotate(&modelMatraix, 45.0, 1.0, 1.0, 0.0);

    esMatrixMultiply(&mvpMatrix, &modelMatraix, &persinMatrix);
    eglSwapBuffers(disPlay, eglWindow);
}

void NewCuberDrawer::draw() {
    // Set the viewport
    glViewport(0, 0, viewWidth, viewHeight);

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the program object
    glUseProgram(mProgramObject);

    // Load the vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT,
                          GL_FALSE, 3 * sizeof(GLfloat), verticals);

    glEnableVertexAttribArray(0);

    // Set the vertex color to red
    glVertexAttrib4f(1, 1.0f, 0.0f, 0.0f, 1.0f);

    // Load the MVP matrix
    glUniformMatrix4fv(MAP_LOCATION, 1, GL_FALSE, (GLfloat *) &mvpMatrix.m[0][0]);

    // Draw the cube
    glDrawElements(GL_TRIANGLES, indecsSize, GL_UNSIGNED_INT, indics);
    eglSwapBuffers(disPlay, eglWindow);
}

NewCuberDrawer::~NewCuberDrawer() {

}