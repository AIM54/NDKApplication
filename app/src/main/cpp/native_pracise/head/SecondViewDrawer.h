//
// Created by Administrator on 2019/11/20.
//

#ifndef NDKAPPLICATION_SECONDVIEWDRAWER_H
#define NDKAPPLICATION_SECONDVIEWDRAWER_H

#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

#include "BaseOpenGlDrawer.h"

class SecondViewDrawer : public BaseOpenGlDrawer {
private:
    void
    initVAO(GLfloat *elementArray, GLshort *indexArray, int stride, int elementSize, int indexSize);

    void drawElement();

    GLuint dataBuffer[2];
    GLuint vertexArray[1];


public:
    SecondViewDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *pJobject);


    int init() override;

    void step() override;

    virtual ~SecondViewDrawer();


};


#endif //NDKAPPLICATION_SECONDVIEWDRAWER_H
