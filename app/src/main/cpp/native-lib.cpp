#include <jni.h>
#include <string>
#include "SuperPlayer.h"
#include "JNICallbackHelper.h"

extern "C"
{
#include "libavutil/avutil.h"
}

// 跨线程通信
JavaVM *vm = nullptr;
jint JNI_OnLoad(JavaVM *vm, void *args) {
    ::vm = vm;
    return JNI_VERSION_1_6;
}

SuperPlayer* player = nullptr;
extern "C"
JNIEXPORT void JNICALL
Java_cn_tim_player_SuperPlayer_prepareNative(JNIEnv *env, jobject job, jstring data_source) {
    const char * data_source_ = env->GetStringUTFChars(data_source, nullptr);
    auto *helper = new JNICallbackHelper(vm, env, job); // C++子线程回调
    player = new SuperPlayer(data_source_, helper);
    player->prepare();
    env->ReleaseStringUTFChars(data_source, data_source_);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_tim_player_SuperPlayer_startNative(JNIEnv *env, jobject thiz) {
    if(player){
        player->start();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_tim_player_SuperPlayer_stopNative(JNIEnv *env, jobject thiz) {
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_tim_player_SuperPlayer_releaseNative(JNIEnv *env, jobject thiz) {
}

extern "C"
JNIEXPORT jstring JNICALL
Java_cn_tim_player_SuperPlayer_getFFmpegVersion(JNIEnv *env, jobject thiz) {
    std::string info  = "FFmpeg version = ";
    info.append(av_version_info());
    return env->NewStringUTF(info.c_str());
}