//
// Created by Administrator on 2019/12/7.
//

#ifndef NDKAPPLICATION_PARTICULARDRAWER_H
#define NDKAPPLICATION_PARTICULARDRAWER_H

#include "BaseOpenGlDrawer.h"
#define NUM_PARTICLES   1000
#define PARTICLE_SIZE   7

#define ATTRIBUTE_LIFETIME_LOCATION       0
#define ATTRIBUTE_STARTPOSITION_LOCATION  1
#define ATTRIBUTE_ENDPOSITION_LOCATION    2


class ParticularDrawer : public BaseOpenGlDrawer {
private:
    GLint timeLoc;
    GLint centerPostionLoc;
    GLint colorLoc;
    GLint sTextureLoc;
    float particleData[NUM_PARTICLES * PARTICLE_SIZE];
    float lastTime;
    GLuint smokeTexture;
    float renderTime;

    void update();

public:
    ParticularDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *pJobject,
                     jobject bitmap);

    int init() override;

    void step() override;


    ~ParticularDrawer();;

};


#endif //NDKAPPLICATION_PARTICULARDRAWER_H
