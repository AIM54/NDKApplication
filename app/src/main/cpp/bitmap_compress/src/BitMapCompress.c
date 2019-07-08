//
// Created by Administrator on 2019/7/4.
//
#include <stdio.h>
#include <BitMapCompress.h>
#include <turbojpeg.h>
#include <android/bitmap.h>
#include <jni.h>
#include <jpeglib.h>
#include <malloc.h>
#include <android/log.h>
#include <stdbool.h>

char *LOG_MARK = "BML";
#define ALOGE(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, LOG_MARK, fmt, ##__VA_ARGS__)

#define ALOGI(fmt, ...) __android_log_print(ANDROID_LOG_INFO, LOG_MARK, fmt, ##__VA_ARGS__)

#define ALOGD(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, LOG_MARK, fmt, ##__VA_ARGS__)

#define ALOGW(fmt, ...) __android_log_print(ANDROID_LOG_WARN, LOG_MARK, fmt, ##__VA_ARGS__)

int registerNativeMethod(JNIEnv *pInterface);

void writeJpegToFile(uint8_t *temp, int width, int height, jint quality, const char *outPutPath) ;

JNINativeMethod method[] = {
        {"compressBitmap", "(Landroid/graphics/Bitmap;Ljava/lang/String;)I", (void *) compressBitmap}
};

int compressBitmap(JNIEnv *env, jclass type,
                   jobject bitmap,
                   jstring dstPath_) {
    AndroidBitmapInfo *androidBitmapInfo = malloc(sizeof(AndroidBitmapInfo));
    AndroidBitmap_getInfo(env, bitmap, androidBitmapInfo);
    ALOGI("theMapWidth:%d |height:%d the format: %d", androidBitmapInfo->width,
          androidBitmapInfo->height, androidBitmapInfo->format);
    void *bitmapPix = NULL;
    AndroidBitmap_lockPixels(env, bitmap, &bitmapPix);
    int color;
    uint8_t *data = malloc(androidBitmapInfo->width * androidBitmapInfo->height * 3);
    uint8_t r, g, b;
    uint8_t *temp = data;
    for (int width = 0; width < androidBitmapInfo->width; width++) {
        for (int height = 0; height < androidBitmapInfo->height; height++) {
            color = *(int *) bitmapPix;
            r = (color >> 16) & 0xFF;
            g = (color >> 8) & 0xFF;
            b = color & 0xFF;
            // 存放颜色,jpeg的格式为bgr,倒过来存
            *data = b;
            *(data + 1) = g;
            *(data + 2) = r;
            data += 3;// 这里就去除了alpha通道
            bitmapPix += 4;// 每个像素有argb4个通道,其中alpha通道被我们丢弃掉了,没有存到data里面
        }
    }
    writeJpegToFile(temp,androidBitmapInfo->width,androidBitmapInfo->height,70,dstPath_);
    AndroidBitmap_unlockPixels(env, bitmap);
    free(androidBitmapInfo);
    return 0;
}

void writeJpegToFile(uint8_t *temp, int width, int height, jint quality, const char *outPutPath) {
////    3.1、创建jpeg压缩对象
//    struct jpeg_compress_struct jcs;
//    //错误回调
//    struct jpeg_error_mgr error;
//    jcs.err = jpeg_std_error(&error);
//    //创建压缩对象
//    jpeg_create_compress(&jcs);
////    3.2、指定存储文件  write binary
//    FILE *f = fopen(outPutPath, "wb");
//    jpeg_stdio_dest(&jcs, f);
////    3.3、设置压缩参数
//    jcs.image_width = width;
//    jcs.image_height = height;
//    //bgr
//    jcs.input_components = 3;
//    jcs.in_color_space = JCS_RGB;
//    jpeg_set_defaults(&jcs);
//    //开启哈夫曼功能
//    jcs.optimize_coding = true;
//    jpeg_set_quality(&jcs, quality, 1);
////    3.4、开始压缩
//    jpeg_start_compress(&jcs, 1);
////    3.5、循环写入每一行数据
//    int row_stride = width * 3;//一行的字节数
//    JSAMPROW row[1];
//    while (jcs.next_scanline < jcs.image_height) {
//        //取一行数据
//        uint8_t *pixels = temp + jcs.next_scanline * row_stride;
//        row[0] = pixels;
//        jpeg_write_scanlines(&jcs, row, 1);
//    }
////    3.6、压缩完成
//    jpeg_finish_compress(&jcs);
////    3.7、释放jpeg对象
//    fclose(f);
//    jpeg_destroy_compress(&jcs);
}


jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if ((*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    //注册方法
    if (registerNativeMethod(env) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}

int registerNativeMethod(JNIEnv *pInterface) {
    jclass jobject = (*pInterface)->FindClass(pInterface,
                                              "com/bian/myapplication/utils/BitMapCompressUtil");
    if (!jobject) {
        return -1;
    }
    if ((*pInterface)->RegisterNatives(pInterface, jobject, method,
                                       sizeof(method) / sizeof(method[0])) < 0) {
        return -1;
    }
    return JNI_OK;
}

