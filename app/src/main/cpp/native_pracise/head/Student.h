//
// Created by Administrator on 2019/7/9.
//

#ifndef NDKAPPLICATION_STUDENT_H
#define NDKAPPLICATION_STUDENT_H


#include <string>

using namespace std;

class Student {
public:
    string name;
    int age;

    void increaseAge() {
        this->age++;
    }
    void testThead();
};


void testMultiThead(Student student);


#endif //NDKAPPLICATION_STUDENT_H
