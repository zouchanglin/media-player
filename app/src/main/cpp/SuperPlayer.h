#ifndef MEDIA_PLAYER_SUPER_PLAYER_H
#define MEDIA_PLAYER_SUPER_PLAYER_H

#include <cstring>
#include <android/log.h>
#include <pthread.h>
#include "AudioChannel.h"
#include "VideoChannel.h"
#include "JNICallbackHelper.h"
#include "myutil.h"

extern "C"
{
#include "libavformat/avformat.h"
};
// 定义输出的TAG
#define LOG_TAG "SuperPlayer"

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


class SuperPlayer {
private:
    char* data_source = nullptr;

    pthread_t pid_prepare;

    AVFormatContext* formatContext = nullptr;

    AudioChannel *audioChannel = nullptr;

    VideoChannel *videoChannel = nullptr;

    JNICallbackHelper *helper = nullptr;

public:
    SuperPlayer(const char *data_source, JNICallbackHelper *pHelper);
    ~SuperPlayer();

    void prepare();
    void prepare_();

    void start();
};


#endif //!MEDIA_PLAYER_SUPER_PLAYER_H
