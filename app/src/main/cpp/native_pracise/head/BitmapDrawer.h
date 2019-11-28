//
// Created by Administrator on 2019/11/23.
//

#ifndef NDKAPPLICATION_BITMAPDRAWER_H
#define NDKAPPLICATION_BITMAPDRAWER_H

#include <android/bitmap.h>
#include "BaseOpenGlDrawer.h"

#define VERTEX_POS_INDX         0
#define TEXT_POS_INDEX          1
#define VERTEX_POS_SIZE         3
#define TEXT_POS_SIZE           2


class BitmapDrawer : public BaseOpenGlDrawer {
private:

    JNIEnv *mEnv;
    jobject mBitMap;
    GLint sTexture;

protected:
    GLuint dataBuffer[3];

    GLuint bitmapTexture[1];
public:
    BitmapDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *assermanager,
                 jobject bitmap);

    ~BitmapDrawer();

    int init() override;

    void step() override;

    void *pixel_source;
};


#endif //NDKAPPLICATION_BITMAPDRAWER_H
