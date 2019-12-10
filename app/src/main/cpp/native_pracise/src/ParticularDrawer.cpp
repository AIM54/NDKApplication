//
// Created by Administrator on 2019/12/7.
//

#include "ParticularDrawer.h"
#include "esUtil.h"

extern "C" {
#include "TimeUtil.h"
#include "AssetReader.h"
}
ParticularDrawer::ParticularDrawer(JNIEnv *jniEnv, const _jobject *surface,
                                   const _jobject *pJobject, jobject bitmap) : BaseOpenGlDrawer(
        jniEnv, const_cast<jobject>(surface), const_cast<jobject>(pJobject)) {
    AndroidBitmapInfo infoColor;
    AndroidBitmap_getInfo(jniEnv, bitmap, &infoColor);
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

int ParticularDrawer::init() {
    if (!BaseOpenGlDrawer::init()) {
        ALOGI("call super method failed");
        return -1;
    }

    initProgram("first_particular_v.glsl", "first_particular_fragment.glsl");
    if (!mProgramObject) {
        return -1;
    }
    timeLoc = glGetUniformLocation(mProgramObject, "u_time");
    if (timeLoc < 0) {
        ALOGI("failed to get timeLoc");
        return -1;
    }
    centerPostionLoc = glGetUniformLocation(mProgramObject, "u_centerPosition");
    if (centerPostionLoc < 0) {
        ALOGI("failed to get centerPostionLoc");
        return -1;
    }
    colorLoc = glGetUniformLocation(mProgramObject, "u_Color");
    if (colorLoc < 0) {
        ALOGI("failed to get colorLoc");
        return -1;
    }
    sTextureLoc = glGetUniformLocation(mProgramObject, "s_texture");
    if (sTextureLoc < 0) {
        ALOGI("failed to get sTextureLoc");
        return -1;
    }
    srand(0);
    for (int index = 0; index < NUM_PARTICLES; ++index) {
        float *particle = &this->particleData[index * PARTICLE_SIZE];
        // 设置粒子的生存时间 一秒以内
        ( *particle++ ) = ( ( float ) ( rand() % 10000 ) / 10000.0f );
        //设置粒子爆炸后的终点位置  xyz坐标的取值范围都在（-1，1）之间
        ( *particle++ ) = ( ( float ) ( rand() % 10000 ) / 5000.0f ) - 1.0f;
        ( *particle++ ) = ( ( float ) ( rand() % 10000 ) / 5000.0f ) - 1.0f;
        ( *particle++ ) = ( ( float ) ( rand() % 10000 ) / 5000.0f ) - 1.0f;
        //设置粒子爆炸那一刻粒子的起点位置  xyz坐标的取值范围都在（-0.125,0.125）之间
        ( *particle++ ) = ( ( float ) ( rand() % 10000 ) / 40000.0f ) - 0.125f;
        ( *particle++ ) = ( ( float ) ( rand() % 10000 ) / 40000.0f ) - 0.125f;
        ( *particle++ ) = ( ( float ) ( rand() % 10000 ) / 40000.0f ) - 0.125f;
    }
    smokeTexture = loadTexture(assetManager, "smoke.tga");
    if (smokeTexture <= 0) {
        ALOGI("failed to load texture");
        return -1;
    }
    renderTime = 1.0f;
    lastTime = getCurrentTime();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    return GL_TRUE;
}

void ParticularDrawer::update() {
    float curTime = getCurrentTime();
    float deltaTime = (curTime - lastTime);
    lastTime = curTime;
    renderTime += deltaTime;
    if (renderTime >= 1.0) {
        float centerPos[3];
        float color[4];


        // Pick a new start location and color
        centerPos[0] = 0.0f;
        centerPos[1] =  0.0f;
        centerPos[2] = 0.0f;

        glUniform3fv ( centerPostionLoc, 1, &centerPos[0] );

        // Random color
        color[0] = ( ( float ) ( rand() % 10000 ) / 20000.0f ) + 0.5f;
        color[1] = ( ( float ) ( rand() % 10000 ) / 20000.0f ) + 0.5f;
        color[2] = ( ( float ) ( rand() % 10000 ) / 20000.0f ) + 0.5f;
        color[3] = 0.5;

        glUniform4fv ( colorLoc, 1, &color[0] );
    }

    glUniform1i(this->timeLoc, this->renderTime);
}

void ParticularDrawer::step() {
    if (!mProgramObject) {
        return;
    }
    update();

    glViewport(0, 0, viewWidth, viewHeight);
    glEnable(GL_BLEND);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(mProgramObject);

    glVertexAttribPointer(ATTRIBUTE_LIFETIME_LOCATION, 1, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * PARTICLE_SIZE, particleData);

    glVertexAttribPointer(ATTRIBUTE_ENDPOSITION_LOCATION, 3, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * PARTICLE_SIZE, &particleData[1]);

    glVertexAttribPointer(ATTRIBUTE_STARTPOSITION_LOCATION, 3, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * PARTICLE_SIZE, &particleData[4]);

    glEnableVertexAttribArray(ATTRIBUTE_LIFETIME_LOCATION);
    glEnableVertexAttribArray(ATTRIBUTE_ENDPOSITION_LOCATION);
    glEnableVertexAttribArray(ATTRIBUTE_STARTPOSITION_LOCATION);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ALPHA, GL_ONE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, smokeTexture);
    glUniform1i(sTextureLoc, 0);

    glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
    eglSwapBuffers(disPlay, eglWindow);
}

ParticularDrawer::~ParticularDrawer() {
    delete[]pixelColor;
    glDeleteTextures(1, &smokeTexture);
    glDeleteProgram(mProgramObject);

}