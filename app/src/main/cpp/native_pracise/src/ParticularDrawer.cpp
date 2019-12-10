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
    centerPostionLoc = glGetUniformLocation(mProgramObject, "u_centerPosition");
    colorLoc = glGetUniformLocation(mProgramObject, "u_Color");
    sTextureLoc = glGetUniformLocation(mProgramObject, "s_texture");
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    srand(0);
    int index;
    for (index = 0; index < NUM_PARTICLES; ++index) {
        float *particle = &this->particleData[index * PARTICLE_SIZE];
        // 设置粒子的生存时间 一秒以内
        (*particle++) = ((float) (rand() % 10000) / 10000.0f);
        //设置粒子爆炸后的终点位置  xyz坐标的取值范围都在（-1，1）之间
        (*particle++) = ((float) (rand() % 10000) / 5000.0f) - 1.0f;
        (*particle++) = ((float) (rand() % 10000) / 5000.0f) - 1.0f;
        (*particle++) = ((float) (rand() % 10000) / 5000.0f) - 1.0f;
        //设置粒子爆炸那一刻粒子的起点位置  xyz坐标的取值范围都在（-0.125,0.125）之间
        (*particle++) = ((float) (rand() % 10000) / 40000.0f) - 0.125f;
        (*particle++) = ((float) (rand() % 10000) / 40000.0f) - 0.125f;
        (*particle++) = ((float) (rand() % 10000) / 40000.0f) - 0.125f;
    }
    renderTime = 1.0f;

    smokeTexture = loadTexture(assetManager, "smoke.tga");

    if (smokeTexture <= 0) {
        return FALSE;
    }
    ALOGI("init");
    return TRUE;
}

void ParticularDrawer::update() {
    float curTime = getCurrentTime();
    float deltaTime = (curTime - lastTime);
    lastTime = curTime;
    this->renderTime += deltaTime;
    glUseProgram(mProgramObject);

    if (renderTime >= 1.0f) {
        float centerPos[3];
        float color[4];

        this->renderTime = 0.0f;

        // Pick a new start location and color
        centerPos[0] = 0.0f;
        centerPos[1] = 0.0f;
        centerPos[2] = 0.0f;

        glUniform3fv(centerPostionLoc, 1, &centerPos[0]);

        // Random color
        color[0] = ((float) (rand() % 10000) / 20000.0f) + 0.5f;
        color[1] = ((float) (rand() % 10000) / 20000.0f) + 0.5f;
        color[2] = ((float) (rand() % 10000) / 20000.0f) + 0.5f;
        color[3] = 0.5;

        glUniform4fv(colorLoc, 1, &color[0]);
    }

    glUniform1i(this->timeLoc, this->renderTime);
}

void ParticularDrawer::step() {
    if (!mProgramObject) {
        return;
    }
    update();
    // Set the viewport
    glViewport(0, 0, viewWidth, viewHeight);

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    glVertexAttribPointer(ATTRIBUTE_LIFETIME_LOCATION, 1, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * PARTICLE_SIZE, this->particleData);

    glVertexAttribPointer(ATTRIBUTE_ENDPOSITION_LOCATION, 3, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * PARTICLE_SIZE, &this->particleData[1]);

    glVertexAttribPointer(ATTRIBUTE_STARTPOSITION_LOCATION, 3, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * PARTICLE_SIZE, &this->particleData[4]);

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
    glDeleteTextures(1, &smokeTexture);
    glDeleteProgram(mProgramObject);

}