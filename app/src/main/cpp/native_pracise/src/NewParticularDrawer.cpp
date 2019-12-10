//
// Created by Administrator on 2019/12/8.
//

#include <esUtil.h>
#include "NewParticularDrawer.h"

extern "C" {
#include "TimeUtil.h"
#include "AssetReader.h"
}


NewParticularDrawer::NewParticularDrawer(JNIEnv *jniEnv, const _jobject *surface,
                                         const _jobject *pJobject) : BaseOpenGlDrawer(jniEnv,
                                                                                      const_cast<jobject>(surface),
                                                                                      const_cast<jobject>(pJobject)) {

}

int NewParticularDrawer::init() {
    if (!BaseOpenGlDrawer::init()) {
        ALOGI("call super method failed");
        return -1;
    }
    initProgram("first_particular_v.glsl", "first_particular_fragment.glsl");
    if (!mProgramObject) {
        return -1;
    }
    timeLoc = glGetUniformLocation(mProgramObject, "u_time");
    centerPositionLoc = glGetUniformLocation(mProgramObject, "u_centerPosition");
    colorLoc = glGetUniformLocation(mProgramObject, "u_Color");
    samplerLoc = glGetUniformLocation(mProgramObject, "s_texture");
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    srand(0);
    int i;
    for (i = 0; i < NUM_PARTICLES; i++) {
        float *particleData = &this->particleData[i * PARTICLE_SIZE];

        // Lifetime of particle
        (*particleData++) = ((float) (rand() % 10000) / 10000.0f);

        // End position of particle
        (*particleData++) = ((float) (rand() % 10000) / 5000.0f) - 1.0f;
        (*particleData++) = ((float) (rand() % 10000) / 5000.0f) - 1.0f;
        (*particleData++) = ((float) (rand() % 10000) / 5000.0f) - 1.0f;

        // Start position of particle
        (*particleData++) = ((float) (rand() % 10000) / 40000.0f) - 0.125f;
        (*particleData++) = ((float) (rand() % 10000) / 40000.0f) - 0.125f;
        (*particleData++) = ((float) (rand() % 10000) / 40000.0f) - 0.125f;
    }
    time = 1.0f;

    textureId = loadTexture(assetManager, "smoke.tga");

    if (textureId <= 0) {
        return FALSE;
    }
    ALOGI("init");
    return TRUE;
}

void NewParticularDrawer::update() {
    float curTime = getCurrentTime();
    float deltaTime = (curTime - lastTime);
    lastTime = curTime;
    this->time += deltaTime;

    glUseProgram(this->mProgramObject);

    if (this->time >= 1.0f) {
        float centerPos[3];
        float color[4];

        this->time = 0.0f;

        // Pick a new start location and color
        centerPos[0] = ((float) (rand() % 10000) / 10000.0f) - 0.5f;
        centerPos[1] = ((float) (rand() % 10000) / 10000.0f) - 0.5f;
        centerPos[2] = ((float) (rand() % 10000) / 10000.0f) - 0.5f;

        glUniform3fv(this->centerPositionLoc, 1, &centerPos[0]);

        // Random color
        color[0] = ((float) (rand() % 10000) / 20000.0f) + 0.5f;
        color[1] = ((float) (rand() % 10000) / 20000.0f) + 0.5f;
        color[2] = ((float) (rand() % 10000) / 20000.0f) + 0.5f;
        color[3] = 0.5;

        glUniform4fv(this->colorLoc, 1, &color[0]);
    }

    // Load uniform time variable
    glUniform1f(this->timeLoc, this->time);
}

void NewParticularDrawer::step() {
    update();
    // Set the viewport
    glViewport(0, 0, viewWidth, viewHeight);

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the program object
    glUseProgram(mProgramObject);

    // Load the vertex attributes
    glVertexAttribPointer(ATTRIBUTE_LIFETIME_LOCATION, 1, GL_FLOAT,
                          GL_FALSE, PARTICLE_SIZE * sizeof(GLfloat),
                          this->particleData);

    glVertexAttribPointer(ATTRIBUTE_ENDPOSITION_LOCATION, 3, GL_FLOAT,
                          GL_FALSE, PARTICLE_SIZE * sizeof(GLfloat),
                          &this->particleData[1]);

    glVertexAttribPointer(ATTRIBUTE_STARTPOSITION_LOCATION, 3, GL_FLOAT,
                          GL_FALSE, PARTICLE_SIZE * sizeof(GLfloat),
                          &this->particleData[4]);


    glEnableVertexAttribArray(ATTRIBUTE_LIFETIME_LOCATION);
    glEnableVertexAttribArray(ATTRIBUTE_ENDPOSITION_LOCATION);
    glEnableVertexAttribArray(ATTRIBUTE_STARTPOSITION_LOCATION);

    // Blend particles
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->textureId);

    // Set the sampler texture unit to 0
    glUniform1i(this->samplerLoc, 0);

    glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
    eglSwapBuffers(disPlay, eglWindow);

}


