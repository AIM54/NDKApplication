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
    byte *pixelColor;
    if (ANDROID_BITMAP_RESULT_SUCCESS ==
        AndroidBitmap_lockPixels(jniEnv, bitmap, reinterpret_cast<void **>(&pixel_source))) {
        ALOGI("lock pixs success:%d", sizeof(pixelColor));
        ALOGI("bitmap.size:%ld",infoColor.stride*infoColor.height);
        AndroidBitmap_unlockPixels(jniEnv, bitmap);
    }
    ALOGI("the Image width:%d,the Image Height:%d   ImageStride,%ld", infoColor.width, infoColor.height,infoColor.stride);
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
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
    };

    GLfloat textRect[]{
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
    };


    eglSwapBuffers(disPlay, eglWindow);
    ALOGE("step");
}

BitmapDrawer::~BitmapDrawer() {
    glDeleteBuffers(3, dataBuffer);
}


