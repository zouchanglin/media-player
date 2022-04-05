#ifndef MEDIA_PLAYER_SAFE_QUEUE_H
#define MEDIA_PLAYER_SAFE_QUEUE_H

#include <queue>
#include <pthread>

using namespace std;

template<typename T>
class SafeQueue{
private:
    queue<T> queue;
    pthread_mutex_t mutex; // 互斥锁
    pthread_cond_t cond; // 等待/唤醒
    int work; // 标记队列是否工作

    typedef void (*ReleaseCallback)(T *); // 此函数指针用来释放T里面的内容
    ReleaseCallback releaseCallback;
public:
    SafeQueue(){
        pthread_mutex_init(&mutex, 0); //初始化互斥锁
        pthread_cond_init(&cond, 0); // 初始化条件变量
    }

    ~SafeQueue(){
        pthread_mutex_destory(&mutex, 0); //释放互斥锁
        pthread_cond_destory(&cond, 0); // 释放条件变量
    }

    /**
     * 入队
     * @param value
     */
    void insertToQueue(T value){
        pthread_mutex_lock(&mutex); // 多线程访问：先锁住
        if(work){
            queue.push(value);
            pthread_mutex_signal(&cond); // 插入数据后，发出通知唤醒
        } else{
            // 非工作状态，释放value，不知道如何释放，T类型不明确，通过函数指针回调给外界释放
            if(releaseCallback) {
                releaseCallback(&value); // 让外界释放Value
            }
        }

        pthread_mutex_unlock(&mutex); // 释放锁
    }

    /**
     * 出队
     */
    int getQueueAndDel(T &value){
        int ret = 0;
        pthread_mutex_lock(&mutex); // 多线程访问：先锁住

        while(work && queue.empty()){
            // 如果是工作状态 而且没有数据，就在这里进行阻塞
            pthread_cond_wait(&cond, &mutex);
        }

        // 如果有数据就返回数据
        if(!queue.empty()) {
            value = queue.front();
            queue.pop();
            ret = 1; // 表示pop成功
        }

        pthread_mutex_unlock(&mutex); // 释放锁
        return ret;
    }

    /**
     * 设置队列工作状态，是否工作
     */
    void setWork(int work){
        pthread_mutex_lock(&mutex); // 多线程访问：先锁住
        this->work = work;

        // 每次设置状态后，就去唤醒有没有阻塞的出队操作
        pthread_cond_signal(&cond);

        pthread_mutex_unlock(&mutex); // 释放锁
    }

    int empty(){
        return queue.empty();
    }

    int size(){
        return queue.size();
    }
    /**
     * 清除队列
     */
    void clear(){
        pthread_mutex_lock(&mutex); // 多线程访问：先锁住
        int size = queue.size();
        for (int i = 0; i < size; ++i) {
            // 循环释放队列中的数据
            T value = queue.front();
            if(releaseCallback){
                releaseCallback(&value); // 让外界释放堆区空间
            }
            queue.pop();
        }
        pthread_mutex_unlock(&mutex); // 释放锁
    }


    void setReleaseCallback(ReleaseCallback callback){
        this->releaseCallback = callback;
    }
};
#endif //!MEDIA_PLAYER_SAFE_QUEUE_H
