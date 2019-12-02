//
// Created by Administrator on 2019/11/23.
//

#include "BitmapDrawer.h"
#include "AndroidLog.h"

extern "C" {
#include "AssetReader.h"
}

BitmapDrawer::BitmapDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *pJobject,
                           jobject bitmap)
        : BaseOpenGlDrawer(jniEnv, const_cast<jobject>(surface), const_cast<jobject>(pJobject)) {
    AndroidBitmapInfo infoColor;
    AndroidBitmap_getInfo(jniEnv, bitmap, &infoColor);
    imageHeight = infoColor.height;
    imageWidth = infoColor.width;
    pixelColor = nullptr;
    if (ANDROID_BITMAP_RESULT_SUCCESS ==
        AndroidBitmap_lockPixels(jniEnv, bitmap, reinterpret_cast<void **>(&pixel_source))) {
        pixelColor = new byte[infoColor.stride * infoColor.height];
        if (pixelColor) {
            memcpy(pixelColor, pixel_source, infoColor.stride * infoColor.height);
        }
        AndroidBitmap_unlockPixels(jniEnv, bitmap);
    }
}

int BitmapDrawer::init() {
    if (!BaseOpenGlDrawer::init()) {
        ALOGI("call super method failed");
        return -1;
    }
    initProgram("first_bitmap_v.glsl", "first_bitmap_shader.glsl");
    if (!mProgramObject) {
        return -1;
    }
    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    glGenTextures(1, bitmapTexture);
    glBindTexture(GL_TEXTURE_2D, bitmapTexture[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    sTexture = glGetUniformLocation(mProgramObject, "s_texture");
    return 0;
}

void BitmapDrawer::step() {
    if (!mProgramObject) {
        ALOGE("program failed to create!");
        return;
    }
    GLfloat rect[]{
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
    };

    GLfloat textRect[]{
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
    };
    GLshort indices[6] = {0, 1, 2, 2, 0, 3};

    glViewport(0, 0, viewWidth, viewHeight);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bitmapTexture[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 pixelColor);
    glBindTexture(GL_TEXTURE_2D, 0);


    glUseProgram(mProgramObject);
    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                          rect);
    glEnableVertexAttribArray(TEXT_POS_INDEX);
    glVertexAttribPointer(TEXT_POS_INDEX, TEXT_POS_SIZE, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
                          textRect);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bitmapTexture[0]);
    glUniform1i(sTexture, 0);
    glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, indices);
    glBindTexture(GL_TEXTURE_2D, 0);
    eglSwapBuffers(disPlay, eglWindow);
    ALOGE("step");
}

BitmapDrawer::~BitmapDrawer() {
    glDeleteBuffers(3, dataBuffer);
    delete[]pixelColor;
}


