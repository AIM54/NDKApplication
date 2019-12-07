//
// Created by Administrator on 2019/11/30.
//

#ifndef NDKAPPLICATION_CUBEDRAWER_H
#define NDKAPPLICATION_CUBEDRAWER_H
#define VERTEX_POS_INDX            0
#define VERTEX_COLOR_INDX          1
#define MATRIX_POS_INDX            2

#define VERTEX_POS_SIZE            3
#define VERTEX_COLOR_SIZE          4

#define VERTEX_NUMBER             24

#define NUM_INSTANCES_CB              1

#include "BaseOpenGlDrawer.h"

class CubeDrawer : public BaseOpenGlDrawer {
private:

    void update();

public:
    CubeDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *pJobject);

    int init() override;

    void step() override;

    virtual ~CubeDrawer();

    float lastTime;
    int numIndices;
    GLuint indicesIBO;
    GLuint positionVBO;
    GLuint colorVBO;
    GLfloat angle[NUM_INSTANCES_CB];

    GLuint mvpVBO;
};


#endif //NDKAPPLICATION_CUBEDRAWER_H
