//
// Created by Administrator on 2019/12/8.
//

#ifndef NDKAPPLICATION_NEWPARTICULARDRAWER_H
#define NDKAPPLICATION_NEWPARTICULARDRAWER_H

#include "ParticularDrawer.h"

#include "BaseOpenGlDrawer.h"

class NewParticularDrawer : public BaseOpenGlDrawer {
private:
    GLint timeLoc;
    GLint centerPositionLoc;
    GLint colorLoc;
    GLint samplerLoc;
    float particleData[ NUM_PARTICLES * PARTICLE_SIZE ];
    float time;
    GLuint textureId;
    float lastTime;

    void update();
public:
    NewParticularDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *pJobject);

    int init() override;

    void step() override;


};


#endif //NDKAPPLICATION_NEWPARTICULARDRAWER_H
