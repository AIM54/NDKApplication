//
// Created by Administrator on 2019/11/30.
//

#include "CubeDrawer.h"
#include "esUtil.h"

GLfloat cubeColor[] =
        {
                1.0f, 1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 0.0f,

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
        //一旦缓冲好数据后就可以，把相应的数据给是释放掉了
        free(verticals);


        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColor), cubeColor, GL_STATIC_DRAW);


        glGenBuffers(1, &indicsBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicsBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indecsSize, indics, GL_STATIC_DRAW);
        free(indics);
        //这里和下面的代码都很重要，代表了绑定缓冲区完成，这个操作
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

void CubeDrawer::update() {

    ESMatrix perspectMatrix;

    ESMatrix modelMatrix;

    GLfloat aspect = (GLfloat) viewWidth / (GLfloat) viewHeight;

    esMatrixLoadIdentity(&perspectMatrix);
    esPerspective(&perspectMatrix, 60.0f, aspect, 1.0f, 20.0f);

    glBindBuffer(GL_ARRAY_BUFFER, matrixBuffer);

    ESMatrix *matrix = static_cast<ESMatrix *>(glMapBufferRange(GL_ARRAY_BUFFER, 0,
                                                                sizeof(ESMatrix),
                                                                GL_MAP_WRITE_BIT));

    esMatrixLoadIdentity(&modelMatrix);

    esRotate(&modelMatrix, 60.0f, 1.0f, 0.0f, 1.0f);

    esMatrixMultiply(&matrix[0], &modelMatrix, &perspectMatrix);

    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void CubeDrawer::draw() {
    glViewport(0, 0, viewWidth, viewHeight);
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mProgramObject);

    glBindBuffer(GL_ARRAY_BUFFER, postionBuffer);
    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * VERTEX_POS_SIZE, (const void *) NULL);

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glEnableVertexAttribArray(VERTEX_COLOR_INDX);
    glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * VERTEX_COLOR_SIZE, (const void *) NULL);


    glBindBuffer(GL_ARRAY_BUFFER, matrixBuffer);
    //要想矩阵起效，就要一行一行的enable
    glEnableVertexAttribArray(MATRIX_POS_INDX + 0);
    glVertexAttribPointer(MATRIX_POS_INDX + 0, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix),
                          (const void *) NULL);

    glEnableVertexAttribArray(MATRIX_POS_INDX + 1);
    glVertexAttribPointer(MATRIX_POS_INDX + 1, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix),
                          (const void *) (sizeof(GLfloat) * 4));

    glEnableVertexAttribArray(MATRIX_POS_INDX + 2);
    glVertexAttribPointer(MATRIX_POS_INDX + 2, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix),
                          (const void *) (sizeof(GLfloat) * 8));

    glEnableVertexAttribArray(MATRIX_POS_INDX + 3);
    glVertexAttribPointer(MATRIX_POS_INDX + 3, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix),
                          (const void *) (sizeof(GLfloat) * 12));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicsBuffer);

    glDrawElements(GL_TRIANGLES, indecsSize, GL_UNSIGNED_INT, 0);
    eglSwapBuffers(disPlay, eglWindow);
    ALOGI("the indecSize:%d", indecsSize);

}

CubeDrawer::~CubeDrawer() {

    glDeleteBuffers(1, &matrixBuffer);

    glDeleteBuffers(1, &indicsBuffer);

    glDeleteBuffers(1, &colorBuffer);

    glDeleteBuffers(1, &postionBuffer);

    glDeleteProgram(mProgramObject);


}



