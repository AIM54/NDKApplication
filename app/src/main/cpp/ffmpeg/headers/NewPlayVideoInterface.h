//
// Created by Administrator on 2019/7/12.
//

#ifndef NDKAPPLICATION_NEWPLAYVIDEO_H
#define NDKAPPLICATION_NEWPLAYVIDEO_H

#include <string>
#include <pthread.h>

enum class PlayState {
    playing, paused, stoped, readyForPlay, error
};

class NewPlayVideoInterface {
public:
    bool isPlayingVideo;
    std::string theVideoUrl;

    virtual void openInput(std::string);

    virtual void playTheVideo();

    virtual void pauseTheVideo();

    virtual void seekToPosition(int position);

    virtual void resumePlay();

    virtual void release();

    void setCurrentPlayState();


    PlayState getCurentPlayState() {
        return currentPlayState;
    }

private:
    std::string playerName;
    PlayState currentPlayState;
};

#endif //NDKAPPLICATION_NEWPLAYVIDEO_H
