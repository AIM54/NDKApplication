//
// Created by Administrator on 2019/8/18.
//

#ifndef NDKAPPLICATION_AUDIOFRAMEDATABEAN_H
#define NDKAPPLICATION_AUDIOFRAMEDATABEAN_H


#include <cstddef>
#include <cstdint>
#include <string>

class AudioFrameDataBean {
private :
    size_t packetSize;
    uint8_t *data;
    double timeStamp;
public:
    std::string deleteName;

    AudioFrameDataBean();

    AudioFrameDataBean(size_t size, uint8_t *paramData);

    AudioFrameDataBean(const AudioFrameDataBean &audioFrameDataBean);

    AudioFrameDataBean &operator=(const AudioFrameDataBean &audioFrameDataBean);

    size_t getSize();

    uint8_t *getData();

    double getTimeStamp();

    void setTimeStamp(double timeStamp);

    virtual ~AudioFrameDataBean();
};


#endif //NDKAPPLICATION_AUDIOFRAMEDATABEAN_H
