//
// Created by Administrator on 2019/12/12.
//

#ifndef NDKAPPLICATION_CUBEBITMAPDRAWER_H
#define NDKAPPLICATION_CUBEBITMAPDRAWER_H

#define INDEX_OF_POSTION        0
#define INDEX_OF_TEXTURE         1
#define INDEX_OF_MATRIX         2

#define SIZE_OF_TEXTURE_DATA              2

#define SIZE_OF_VERTICAL_DATA              3

#define NUMBER_OF_VERTICAL                 24

#define NUMBER_OF_INDEX                    36

#include "BaseOpenGlDrawer.h"
#include <android/bitmap.h>

class CubeBitMapDrawer : public BaseOpenGlDrawer {

private:
    int numIndices;

    uint8_t *bitmapData;

    GLuint verticalBuffer;
    GLuint texturePostionBuffer;
    GLuint indicsBuffer;
    GLuint mvpBuffer;

    GLuint bitmapTexture;
    GLint aTexture;
public:
    CubeBitMapDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *pJobject,
                     jobject bitmap);

    void step() override;

    int init() override;


    void update();

    float lastTime;
    GLfloat angle;
};


#endif //NDKAPPLICATION_CUBEBITMAPDRAWER_H
