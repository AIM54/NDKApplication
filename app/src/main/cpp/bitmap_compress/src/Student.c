//
// Created by Administrator on 2019/7/15.
//
#include <Student.h>
#include <stdio.h>
#include <android/log.h>
#include <malloc.h>

void testStudent() {
    Student *student = malloc(sizeof(Student));
    student->name = "tomcat";
    student->age = 232;
    __android_log_print(ANDROID_LOG_INFO, "BML", "student's name %s||student's age %d",
                        student->name, student->age);
    free(student);
}