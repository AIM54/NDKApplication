//
// Created by Administrator on 2019/7/31.
//

#ifndef NDKAPPLICATION_BITMAPCOMPRESSCPP_H
#define NDKAPPLICATION_BITMAPCOMPRESSCPP_H


#include <jni.h>
#include <stdio.h>
class BitMapCompressCpp {

};
void writeJpegToFile(uint8_t *temp, int width, int height, jint quality, const char *outPutPath);

#endif //NDKAPPLICATION_BITMAPCOMPRESSCPP_H
