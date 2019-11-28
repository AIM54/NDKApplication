//
// Created by Administrator on 2019/11/22.
//

#include "ImageDrawer.h"
#include "turbojpeg.h"
#include "jpeglib.h"

ImageDrawer::ImageDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *pJobject,
                         char *path)
        : BaseOpenGlDrawer(jniEnv, const_cast<jobject>(surface), const_cast<jobject>(pJobject)) {
    this->imagePath = path;
}

int ImageDrawer::init() {
    jpeg_decompress_struct jpegInfo;
    jpeg_error_mgr jpegError;
    FILE *input_file;
    JSAMPARRAY buffer;
    int row_width;

    unsigned char *pixel;
    input_file = fopen(imagePath, "rb");
    if (!input_file) {
        return -1;
    }
    //初始化对象信息
    jpeg_create_decompress(&jpegInfo);
    //指定图片
    jpeg_stdio_src(&jpegInfo, input_file);
    ALOGI("before_read_header");
    //读取文件头信息,设置默认的解压参数
    jpeg_read_header(&jpegInfo, TRUE);
    ALOGI("after_read_header");
    //开始解压
    jpeg_start_decompress(&jpegInfo);

    row_width = jpegInfo.output_width * jpegInfo.output_components;

    buffer = (*jpegInfo.mem->alloc_sarray)((j_common_ptr) &jpegInfo, JPOOL_IMAGE,
                                           row_width, 1);

    //一行
    pixel = (unsigned char *) malloc(row_width);
    memset(pixel, 0, row_width);

    free(pixel);
    //完成解压
    jpeg_finish_decompress(&jpegInfo);

    //销毁解压相关信息
    jpeg_destroy_decompress(&jpegInfo);

    //关闭文件句柄
    fclose(input_file);
    ALOGE("close file");
    return 1;
}

void ImageDrawer::step() {

}
