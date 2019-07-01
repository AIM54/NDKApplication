//
// Created by Administrator on 2019/6/21.
//
#include <android/log.h>
#ifndef NDKAPPLICATION_GLOBALCONFIG_H
#define NDKAPPLICATION_GLOBALCONFIG_H
 char *LOG_MARK = "BML";
#define ALOGE(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, LOG_MARK, fmt, ##__VA_ARGS__)

#define ALOGI(fmt, ...) __android_log_print(ANDROID_LOG_INFO, LOG_MARK, fmt, ##__VA_ARGS__)

#define ALOGD(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, LOG_MARK, fmt, ##__VA_ARGS__)

#define ALOGW(fmt, ...) __android_log_print(ANDROID_LOG_WARN, LOG_MARK, fmt, ##__VA_ARGS__)
#endif //NDKAPPLICATION_GLOBALCONFIG_H


