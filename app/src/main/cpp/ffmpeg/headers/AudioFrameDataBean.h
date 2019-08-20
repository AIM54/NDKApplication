//
// Created by Administrator on 2019/8/18.
//

#ifndef NDKAPPLICATION_AUDIOFRAMEDATABEAN_H
#define NDKAPPLICATION_AUDIOFRAMEDATABEAN_H


#include <cstddef>
#include <cstdint>

class AudioFrameDataBean {
private :
    size_t packetSize;
    uint8_t *data;
    double timeStamp;
public:
    AudioFrameDataBean();

    AudioFrameDataBean(size_t size, uint8_t *paramData);

    size_t getSize();

    uint8_t *getData();

    void setTimeStamp(double timeStamp);
    virtual ~AudioFrameDataBean();
};


#endif //NDKAPPLICATION_AUDIOFRAMEDATABEAN_H
