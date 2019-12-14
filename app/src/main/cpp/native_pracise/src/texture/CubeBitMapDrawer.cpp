//
// Created by Administrator on 2019/12/12.
//

extern "C" {
#include "TimeUtil.h"
}

#include <InstancingDrawer.h>
#include "CubeBitMapDrawer.h"
#include "esUtil.h"

CubeBitMapDrawer::CubeBitMapDrawer(JNIEnv *jniEnv, const _jobject *surface,
                                   const _jobject *pJobject, jobject bitmap) : BaseOpenGlDrawer(
        jniEnv,
        const_cast<jobject>(surface),
        const_cast<jobject>(pJobject)) {
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(jniEnv, bitmap, &info);
    uint8_t *pixel_source;
    if (ANDROID_BITMAP_RESULT_SUCCESS ==
        AndroidBitmap_lockPixels(jniEnv, bitmap, reinterpret_cast<void **>(&pixel_source))) {
        bitmapData = new uint8_t[info.stride * info.height];
        if (bitmapData) {
            memcpy(bitmapData, pixel_source, info.stride * info.height);
        }
        AndroidBitmap_unlockPixels(jniEnv, bitmap);
    }
}

int CubeBitMapDrawer::init() {
    if (!BaseOpenGlDrawer::init()) {
        return -1;
    }
    initProgram("cube/cube_bitmap_v.glsl", "cube/cube_bitmap_fragment.glsl");
    lastTime = getCurrentTime();
    if (!mProgramObject) {
        ALOGI("failed to init ProgramObject");
    }
    GLfloat *positions;
    GLfloat *textures;
    GLuint *indices;

    // Generate the vertex data
    numIndices = esGenCube(1.0f, &positions,
                           NULL, &textures, &indices);

    glGenBuffers(1, &indicsBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicsBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
    free(indices);


    glGenBuffers(1, &verticalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, verticalBuffer);
    glBufferData(GL_ARRAY_BUFFER, SIZE_OF_VERTICAL_DATA * NUMBER_OF_VERTICAL * sizeof(GLfloat),
                 positions, GL_STATIC_DRAW);
    free(positions);

    glGenBuffers(1, &texturePostionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texturePostionBuffer);
    glBufferData(GL_ARRAY_BUFFER, SIZE_OF_TEXTURE_DATA * NUMBER_OF_VERTICAL * sizeof(GLfloat),
                 textures, GL_STATIC_DRAW);
    free(textures);

    glGenBuffers(1, &mvpBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mvpBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ESMatrix), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glGenTextures(1, &bitmapTexture);
    glBindTexture(GL_TEXTURE_2D, bitmapTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    aTexture = glGetUniformLocation(mProgramObject, "a_texture");
    if (aTexture < 0) {
        ALOGI("failed to get Texture Postion");
        return GL_FALSE;
    }
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    return GL_TRUE;
}

void CubeBitMapDrawer::step() {
    // Draw the cubes
    update();
    // Set the viewport
    glViewport(0, 0, viewWidth, viewHeight);

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the program object
    glUseProgram(mProgramObject);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bitmapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewWidth, viewHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 bitmapData);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindBuffer(GL_ARRAY_BUFFER, verticalBuffer);
    glVertexAttribPointer(INDEX_OF_POSTION, SIZE_OF_VERTICAL_DATA, GL_FLOAT, GL_FALSE,
                          sizeof(GL_FLOAT) * SIZE_OF_VERTICAL_DATA, (const void *) NULL);
    glEnableVertexAttribArray(INDEX_OF_POSTION);

    glBindBuffer(GL_ARRAY_BUFFER, texturePostionBuffer);
    glVertexAttribPointer(INDEX_OF_TEXTURE, SIZE_OF_TEXTURE_DATA, GL_FLOAT, GL_FALSE,
                          sizeof(GL_FLOAT) * SIZE_OF_TEXTURE_DATA, (const void *) NULL);
    glEnableVertexAttribArray(INDEX_OF_TEXTURE);

    // Load the instance MVP buffer
    glBindBuffer(GL_ARRAY_BUFFER, mvpBuffer);
    // Load each matrix row of the MVP.  Each row gets an increasing attribute location.
    glVertexAttribPointer(INDEX_OF_MATRIX + 0, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix),
                          (const void *) NULL);
    glVertexAttribPointer(INDEX_OF_MATRIX + 1, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix),
                          (const void *) (sizeof(GLfloat) * 4));
    glVertexAttribPointer(INDEX_OF_MATRIX + 2, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix),
                          (const void *) (sizeof(GLfloat) * 8));
    glVertexAttribPointer(INDEX_OF_MATRIX + 3, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix),
                          (const void *) (sizeof(GLfloat) * 12));

    glEnableVertexAttribArray(INDEX_OF_MATRIX + 0);
    glEnableVertexAttribArray(INDEX_OF_MATRIX + 1);
    glEnableVertexAttribArray(INDEX_OF_MATRIX + 2);
    glEnableVertexAttribArray(INDEX_OF_MATRIX + 3);

    // One MVP per instance
    glVertexAttribDivisor(INDEX_OF_MATRIX + 0, 1);
    glVertexAttribDivisor(INDEX_OF_MATRIX + 1, 1);
    glVertexAttribDivisor(INDEX_OF_MATRIX + 2, 1);
    glVertexAttribDivisor(INDEX_OF_MATRIX + 3, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bitmapTexture);
    glUniform1i(aTexture, 0);

    // Bind the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicsBuffer);

    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, (const void *) NULL);

    eglSwapBuffers(disPlay, eglWindow);
}

void CubeBitMapDrawer::update() {
    ESMatrix *matrixBuf;
    ESMatrix perspective;
    float aspect;
    // Compute the window aspect ratio
    aspect = (GLfloat) viewWidth / (GLfloat) viewHeight;

    // Generate a perspective matrix with a 60 degree FOV
    esMatrixLoadIdentity(&perspective);
    esPerspective(&perspective, 60.0f, aspect, 1.0f, 20.0f);

    glBindBuffer(GL_ARRAY_BUFFER, mvpBuffer);
    matrixBuf = (ESMatrix *) glMapBufferRange(GL_ARRAY_BUFFER, 0,
                                              sizeof(ESMatrix),
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

    esMatrixMultiply(&matrixBuf[0], &modelview, &perspective);
    glEnable(GL_DEPTH_TEST);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}
