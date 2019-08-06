//
// Created by Administrator on 2019/8/5.
//

#ifndef NDKAPPLICATION_ARRAYLIST_H
#define NDKAPPLICATION_ARRAYLIST_H

template<class T>
class ArrayList {
public:
    ArrayList(int size = 16);

    void pushValue(T data);

    T getValueAtIndex(int index);

    virtual int getListSize();
    virtual T* getDataArray();

    bool isEmpty();

    virtual ~ArrayList();

private:
    T *data;
    int currentIndex;
};

template<class T>
ArrayList<T>::ArrayList(int size) {
    if (!data) {
        data = new T[size];
        currentIndex=0;
    }
}

template<class T>
ArrayList<T>::~ArrayList() {
    if (!data) {
        delete[] data;
    }
}
template<class T>
void ArrayList<T>::pushValue(T data) {
    this->data[currentIndex]=data;
    currentIndex++;
}

template<class T>
T ArrayList<T>::getValueAtIndex(int index) {

}
template<class T>
int ArrayList<T>::getListSize() {
    return currentIndex+1;
}

template<class T>
bool ArrayList<T>::isEmpty() {
    return currentIndex==0;
}
template<class T>
T* ArrayList<T>::getDataArray() {
    return data;
}
#endif //NDKAPPLICATION_ARRAYLIST_H
