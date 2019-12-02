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

#include "BaseOpenGlDrawer.h"

class CubeDrawer : public BaseOpenGlDrawer {
private:
    GLfloat *verticals;
    GLuint *indics;
    GLuint postionBuffer;
    GLuint indicsBuffer;
    GLuint matrixBuffer;
    GLuint colorBuffer;
    int indecsSize;

    bool hasDraw;

    void draw();

    void update();

public:
    CubeDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *pJobject);

    int init() override;

    void step() override;

};



#endif //NDKAPPLICATION_CUBEDRAWER_H
