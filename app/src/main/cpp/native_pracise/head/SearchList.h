//
// Created by Administrator on 2019/8/5.
//

#ifndef NDKAPPLICATION_SEARCHLIST_H
#define NDKAPPLICATION_SEARCHLIST_H

#include "ArrayList.h"
#include "Comparator.h"

template<class T>
class SearchList : public ArrayList<T> {
private:
    Comparator *mComparator;
public:
    template<class T>
    SearchList(int initSize, Comparator *com);

    ~SearchList();
};

template<class T>
SearchList<T>::SearchList(int initSize, Comparator *com):ArrayList::ArrayList(initSize),
                                                         mComparator(com) {

}

template<class T>
SearchList<T>::~SearchList() {
    if (!mComparator) {
        delete mComparator;
    }
}


#endif //NDKAPPLICATION_SEARCHLIST_H
