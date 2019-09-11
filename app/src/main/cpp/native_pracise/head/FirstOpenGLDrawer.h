//
// Created by Administrator on 2019/9/10.
//

#ifndef NDKAPPLICATION_FIRSTOPENGLDRAWER_H
#define NDKAPPLICATION_FIRSTOPENGLDRAWER_H


#include <android/asset_manager.h>

class FirstOpenGLDrawer {
private:
    AAssetManager *g_pAssetManager = nullptr;
public:
    FirstOpenGLDrawer();

    void reize();

    void step();

    void setAssertManger(AAssetManager *manager);
};

char *getStringFromAssert(AAssetManager *aAssetManager,char * path);


#endif //NDKAPPLICATION_FIRSTOPENGLDRAWER_H
