//
// Created by Administrator on 2019/8/5.
//
#include <iostream>
#include "ArrayList.h"
#include <jni.h>
#include <AndroidLog.h>
#include <vector>
#include <map>

using std::string;

ArrayList<string> *getStringListBySize(int totalSize, string value) {
    ArrayList<string> *stringList = new ArrayList<string>(totalSize);
    for (int index = 0; index < totalSize; index++) {
        stringList->pushValue(value);
    }
    ALOGI("current List.size:%d", stringList->getListSize());
    return stringList;
}