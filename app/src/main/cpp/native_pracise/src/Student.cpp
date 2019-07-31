//
// Created by Administrator on 2019/7/9.
//
#include <Student.h>
#include <string>
#include <string.h>
#include <iostream>

using namespace std;

void testMultiThead(Student student) {
    string name = "tomcat";
    student.name = "lannister";
    student.age = 1;
}

void Student::testThead() {

}



