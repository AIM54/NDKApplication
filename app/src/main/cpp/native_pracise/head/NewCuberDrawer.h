//
// Created by Administrator on 2019/12/5.
//

#ifndef NDKAPPLICATION_NEWCUBERDRAWER_H
#define NDKAPPLICATION_NEWCUBERDRAWER_H

#include "BaseOpenGlDrawer.h"
#include "esUtil.h"

#define  MAP_LOCATION 2

class NewCuberDrawer : public BaseOpenGlDrawer {
private:
private:
    GLfloat *verticals;
    GLuint *indics;
    int indecsSize;
    // MVP matrix
    ESMatrix mvpMatrix;

    bool hasDraw;

    void draw();

    void update();

public:
    NewCuberDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *pJobject);

    int init() override;

    void step() override;

    virtual ~NewCuberDrawer();

};


#endif //NDKAPPLICATION_NEWCUBERDRAWER_H
