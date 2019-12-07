#include <linux/time.h>
#include <time.h>
#include "TimeUtil.h"

//
// Created by Administrator on 2019/12/7.
//
float getCurrentTime() {
    struct timespec clockRealTime;
    clock_gettime(CLOCK_MONOTONIC, &clockRealTime);
    double curTimeInSeconds = clockRealTime.tv_sec + (double) clockRealTime.tv_nsec / 1e9;
    return (float) curTimeInSeconds;
}
