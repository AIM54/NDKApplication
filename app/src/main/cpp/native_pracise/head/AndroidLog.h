//
// Created by Administrator on 2019/7/15.
//

#ifndef NDKAPPLICATION_ANDROIDLOG_H
#define NDKAPPLICATION_ANDROIDLOG_H
#define ALOGE(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, "BML", fmt, ##__VA_ARGS__)

#define ALOGI(fmt, ...) __android_log_print(ANDROID_LOG_INFO, "BML", fmt, ##__VA_ARGS__)

#define ALOGD(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, "BML", fmt, ##__VA_ARGS__)

#define ALOGW(fmt, ...) __android_log_print(ANDROID_LOG_WARN, "BML", fmt, ##__VA_ARGS__)
#endif //NDKAPPLICATION_ANDROIDLOG_H
