//
// Created by Administrator on 2019/1/25.
//
#include "stdio.h"
#include "math.h"
#include "lastlog.h"
#include <stdio.h>
#include "libavcodec/avcodec.h"
typedef struct Agent{
    char *name;
    int age;
    float salary;
}Agent;

int getAddResult(int a, int b){
    avcodec_register_all();
    return a+b;
}

