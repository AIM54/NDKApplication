//
// Created by Administrator on 2019/12/6.
//

#ifndef NDKAPPLICATION_INSTANCINGDRAWER_H
#define NDKAPPLICATION_INSTANCINGDRAWER_H

#include "BaseOpenGlDrawer.h"

#define NUM_INSTANCES   100
#define POSITION_LOC    0
#define COLOR_LOC       1
#define MVP_LOC         2


class InstancingDrawer : public BaseOpenGlDrawer {
private:
    GLuint positionVBO;
    GLuint colorVBO;
    GLuint mvpVBO;
    GLuint indicesIBO;

    // Number of indices
    int numIndices;

    // Rotation angle
    GLfloat angle[NUM_INSTANCES];

    void update();

    void draw();

public:
    InstancingDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *pJobject);

    int init() override;

    void step() override;


    virtual ~InstancingDrawer();

};


#endif //NDKAPPLICATION_INSTANCINGDRAWER_H
