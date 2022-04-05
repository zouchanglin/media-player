#ifndef MEDIA_PLAYER_JNICALLBACK_HELPER_H
#define MEDIA_PLAYER_JNICALLBACK_HELPER_H

#include <jni.h>
#include "myutil.h"

class JNICallbackHelper {
private:
    JavaVM *vm = nullptr;
    JNIEnv *env = nullptr;
    jobject job = nullptr;

    jmethodID jmd_prepared;
    jmethodID jmd_onError;
public:
    JNICallbackHelper(JavaVM *vm, JNIEnv *env, jobject job);

    virtual ~JNICallbackHelper();

    void onPrepared(int thread_mode);

    void onError(int thread_mode, int error_code);
};

#endif //!MEDIA_PLAYER_JNICALLBACK_HELPER_H
