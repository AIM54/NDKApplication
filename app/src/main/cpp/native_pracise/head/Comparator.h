//
// Created by Administrator on 2019/8/5.
//

#ifndef NDKAPPLICATION_COMPARATOR_H
#define NDKAPPLICATION_COMPARATOR_H

template<class T>
class Comparator {
public:
    int compare(T &t1, T &t2);
};

template<class T>
int Comparator<T>::compare(T &t1, T &t2) {
    if (t1 > t2) {
        return 1;
    }
    return t1 == t2 ? 0 : -1;
}


#endif //NDKAPPLICATION_COMPARATOR_H
