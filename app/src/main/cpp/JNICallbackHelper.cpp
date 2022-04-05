#include "JNICallbackHelper.h"

JNICallbackHelper::JNICallbackHelper(JavaVM *vm, JNIEnv *env, jobject job){
    this->vm = vm;
    this->env = env;
//    this->job = job; // jobject 不能跨线程，不能跨函数，必须全局引用
    this->job = env->NewGlobalRef(job); // 提升全局引用

    // 其实env也不能跨线程
    jclass clazz = env-> GetObjectClass(job);
    jmd_prepared = env->GetMethodID(clazz, "onPrePared", "()V");
    jmd_onError = env->GetMethodID(clazz, "onError", "(I)V");
}

JNICallbackHelper::~JNICallbackHelper() {
    this->vm = nullptr;
    env->DeleteGlobalRef(job);
    this->job = nullptr;
    this->env = nullptr;
}

/**
 * 线程模式
 * @param thread_mode
 */
void JNICallbackHelper::onPrepared(int thread_mode) {
    // 判断是否是子线程
    if(thread_mode == THREAD_MAIN){
        // 主线程
        env->CallVoidMethod(job, jmd_prepared);
    }else if(thread_mode == THREAD_CHILD){
        // 子线程 JNI_ENV 必须得有全新的Env
        JNIEnv * env_child;
        vm -> AttachCurrentThread(&env_child, nullptr);

        env_child->CallVoidMethod(job, jmd_prepared);
        vm->DetachCurrentThread();
    }
}

void JNICallbackHelper::onError(int thread_mode, int error_code) {
    // 判断是否是子线程
    if(thread_mode == THREAD_MAIN){
        // 主线程
        env->CallVoidMethod(job, jmd_onError);
    }else if(thread_mode == THREAD_CHILD){
        // 子线程 JNI_ENV 必须得有全新的Env
        JNIEnv * env_child;
        vm -> AttachCurrentThread(&env_child, nullptr);

        env_child->CallVoidMethod(job, jmd_onError);
        vm->DetachCurrentThread();
    }
}
