//
// Created by Administrator on 2019/11/21.
//

#ifndef NDKAPPLICATION_POINTDRAWER_H
#define NDKAPPLICATION_POINTDRAWER_H

#include "BaseOpenGlDrawer.h"
#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

class PointDrawer : public BaseOpenGlDrawer {
private:
    GLuint dataBuffer[2];
    GLuint vertexArray[1];
    void initVAO();
public:
    PointDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *pJobject);

    int init() override;

    void step() override;

};


#endif //NDKAPPLICATION_POINTDRAWER_H
