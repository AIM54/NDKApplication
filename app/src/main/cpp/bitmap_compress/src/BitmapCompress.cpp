//
// Created by Administrator on 2019/11/15.
//
#include <setjmp.h>
#include <cstdio>
#include "turbojpeg.h"
#include "jpeglib.h"
#include "BitmapCompress.h"
#include "AndroidLog.h"
#include <android/bitmap.h>
#include <malloc.h>
#include <stdio.h>
#include <cstring>

typedef uint8_t BYTE;
#define TAG "image "
#define true 1
#define false 0
char *error;
struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr *my_error_ptr;

METHODDEF(void)
my_error_exit(j_common_ptr cinfo) {
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    error = const_cast<char *>(myerr->pub.jpeg_message_table[myerr->pub.msg_code]);
    longjmp(myerr->setjmp_buffer, 1);
}

//图片压缩方法
int generateJPEG(BYTE *data, int w, int h, int quality,
                 const char *outfilename, jboolean optimize) {
    int nComponent = 3;

    struct jpeg_compress_struct jcs;

    struct my_error_mgr jem;

    jcs.err = jpeg_std_error(&jem.pub);
    jem.pub.error_exit = my_error_exit;
    if (setjmp(jem.setjmp_buffer)) {
        return 0;
    }
    //为JPEG对象分配空间并初始化
    jpeg_create_compress(&jcs);
    //获取文件信息
    FILE *f = fopen(outfilename, "wb");
    if (f == NULL) {
        return 0;
    }
    //指定压缩数据源
    jpeg_stdio_dest(&jcs, f);
    jcs.image_width = w;
    jcs.image_height = h;
    jcs.arith_code = false;
    jcs.input_components = nComponent;
    if (nComponent == 1)
        jcs.in_color_space = JCS_GRAYSCALE;
    else
        jcs.in_color_space = JCS_RGB;

    jpeg_set_defaults(&jcs);
    jcs.optimize_coding = optimize;
    //为压缩设定参数，包括图像大小，颜色空间
    jpeg_set_quality(&jcs, quality, true);
    //开始压缩
    jpeg_start_compress(&jcs, TRUE);

    JSAMPROW row_pointer[1];
    int row_stride;
    row_stride = jcs.image_width * nComponent;
    while (jcs.next_scanline < jcs.image_height) {
        row_pointer[0] = &data[jcs.next_scanline * row_stride];
        //写入数据
        jpeg_write_scanlines(&jcs, row_pointer, 1);
    }
    //压缩完毕
    jpeg_finish_compress(&jcs);
    //释放资源
    jpeg_destroy_compress(&jcs);
    fclose(f);

    return 1;
}

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb;

//将java string转换为char*
char *jstringTostring(JNIEnv *env, jbyteArray barr) {
    char *rtn = NULL;
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, 0);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}

int compressBitmap(JNIEnv *env, jclass type, jstring videoUrl_,
                   jobject bitmap) {
    BYTE *pixelColor;
    BYTE *data;
    BYTE *tempData;
    AndroidBitmapInfo infoColor;
    const char *destFilePath = env->GetStringUTFChars(videoUrl_, 0);
    AndroidBitmap_getInfo(env, bitmap, &infoColor);
    if (AndroidBitmap_lockPixels(env, bitmap, reinterpret_cast<void **>(&pixelColor)) < 0) {
        ALOGE("加载失败");
        return 0;
    }
    ALOGI("bitmapInfor imageHeight:%d imageWidth:%d", infoColor.height,
          infoColor.width);
    BYTE r, g, b;
    int color;
    int width = infoColor.width;
    int height = infoColor.height;
    data = static_cast<BYTE *>(malloc(width * height * 3));
    tempData = data;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            color = *((int *) pixelColor);
            r = ((color & 0x00FF0000) >> 16);
            g = ((color & 0x0000FF00) >> 8);
            b = color & 0X000000FF;

            *data = b;
            *(data + 1) = g;
            *(data + 2) = r;
            data += 3;
            pixelColor += 4;
        }
    }

    AndroidBitmap_unlockPixels(env, bitmap);
    int resultCode = generateJPEG(tempData, width, height, 80, destFilePath, true);
    env->ReleaseStringUTFChars(videoUrl_, destFilePath);
    return resultCode;
}


