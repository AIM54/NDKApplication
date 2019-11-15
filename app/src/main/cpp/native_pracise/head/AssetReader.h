//
// Created by Administrator on 2019/11/15.
//

#ifndef NDKAPPLICATION_ASSETMANAGER_H
#define NDKAPPLICATION_ASSETMANAGER_H

#include <android/asset_manager.h>
#include <GLES3/gl3.h>

char *readStringFromAssert(AAssetManager *aAssetManager, char *path);

GLuint loadShader(GLenum type, const char *shaderSrc);

#endif //NDKAPPLICATION_ASSETMANAGER_H
