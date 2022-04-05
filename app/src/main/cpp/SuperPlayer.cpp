#include "SuperPlayer.h"

SuperPlayer::SuperPlayer(const char *data_source, JNICallbackHelper *helper) {
    if(data_source == nullptr) {
        LOGE("data_source is null!");
        return;
    }
    this->data_source = new char[strlen(data_source) + 1];
    strcpy(this->data_source, data_source);

    this->helper = helper;
}

SuperPlayer::~SuperPlayer() {
    delete [] data_source;
    delete helper;
}

void* task_prepare(void * args){
    auto *player = static_cast<SuperPlayer *>(args);
    player->prepare_();
    return nullptr;
}

void SuperPlayer::prepare_() { // 子线程

//    av_register_all();
    /**
     * 第一步 打开文件流/网络串流
     */
    formatContext = avformat_alloc_context(); // 创建上下文

    AVDictionary * dictionary = nullptr;
    // 单位微秒
    av_dict_set(&dictionary, "timeout", "5000000", 0);
    /**
     * fmt：Mac、Windows麦克风，摄像头等，Android用不到
     */
    int ret = avformat_open_input(&formatContext, this->data_source, nullptr, &dictionary);

    // 释放AVDictionary
    av_dict_free(&dictionary);

    if(ret) {
        // 反馈错误信息，回调给Java
        av_log(nullptr, AV_LOG_ERROR, "avformat_open_input failed，ret = %d, error_info = %s\n",
                ret, av_err2str(ret)); // av_err2str根据错误码返回错误描述
        if(helper){
            helper->onError(THREAD_CHILD, FFMPEG_CAN_NOT_OPEN_URL);
        }
        return;
    }

    /**
     * 第二步 查找音视频媒体流中的信息
     */
     ret = avformat_find_stream_info(formatContext, nullptr);
     if(ret < 0) {
         av_log(nullptr, AV_LOG_ERROR, "avformat_find_stream_info failed，ret = %d, error_info = %s\n",
                 ret, av_err2str(ret));
         if(helper){
             helper->onError(THREAD_CHILD, FFMPEG_CAN_NOT_FIND_STREAMS);
             return;
         }
     }

     /**
      * 第三步 根据流信息（流的个数），用循环找
      */
     for (int i = 0; i < formatContext->nb_streams; i++){
         /**
          * 第四步 获取媒体流（音频流/视频流）
          */
         AVStream *stream = formatContext->streams[i];

         /**
          * 第五步 从上面的流中获取编解码参数
          * 后面的编码器和解码器都需要参数(宽高)
          */
         AVCodecParameters * parameters = stream->codecpar;

         /**
          * 第六步 根据上面的参数获取解码器
          */
         AVCodec *codec = avcodec_find_decoder(parameters->codec_id);
         if(!codec){
             av_log(nullptr, AV_LOG_ERROR, "avcodec_find_decoder failed，ret = %d, error_info = %s\n",
                    ret, av_err2str(ret));
             if(helper){
                 helper->onError(THREAD_CHILD, FFMPEG_FIND_DECODER_FAIL);
             }
             return;
         }

         /**
          * 第七步 编解码器上下文（这个才是真正干活的）
          */
          AVCodecContext *codecContext = avcodec_alloc_context3(codec);
          if(!codecContext){
              av_log(nullptr, AV_LOG_ERROR, "avcodec_alloc_context3 failed，ret = %d, error_info = %s\n",
                      ret, av_err2str(ret));
              if(helper){
                  helper->onError(THREAD_CHILD, FFMPEG_ALLOC_CODEC_CONTEXT_FAIL);
              }
              return;
          }
          /**
           * 第八步 parameters copy codecContext
           */
           ret = avcodec_parameters_to_context(codecContext, parameters);
           if(ret < 0) {
               av_log(nullptr, AV_LOG_ERROR, "avcodec_parameters_to_context failed，ret = %d, error_info = %s\n",
                       ret, av_err2str(ret));
               if(helper){
                   helper->onError(THREAD_CHILD, FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL);
               }
               return;
           }

           /**
            * 第九步 打开解码器
            */
            ret = avcodec_open2(codecContext, codec, nullptr);
            if(ret != 0){
                av_log(nullptr, AV_LOG_ERROR, "avcodec_open2 failed，ret = %d, error_info = %s\n",
                        ret, av_err2str(ret));
                if(helper){
                    helper->onError(THREAD_CHILD, FFMPEG_OPEN_DECODER_FAIL);
                }
                return;
            }

            /**
             * 第十步 从编解码器参数中获取流的类型 codec type
             */
             if(parameters->codec_type == AVMediaType::AVMEDIA_TYPE_AUDIO) {
                 // 开启音频通道
                 audioChannel = new AudioChannel();
             } else if(parameters->codec_type == AVMediaType::AVMEDIA_TYPE_VIDEO) {
                 // 开启视频通道
                 videoChannel = new VideoChannel();
             }
     }// for end

     /**
      * 第十一步 如果没有音频、也没有视频 健壮性校验
      */
      if(!audioChannel && !videoChannel){
          av_log(nullptr, AV_LOG_ERROR, "no audioChannel or videoChannel");
          if(helper){
              helper->onError(THREAD_CHILD, FFMPEG_NOMEDIA);
          }
          return;
      }

      /**
       * 第十二步 说明媒体文件准备是OK的
       */
      if(helper) {
          helper->onPrepared(THREAD_CHILD);
      }
}

void SuperPlayer::prepare() {
    // 主线程：MainActivity的onResume调用下来的

    // void* (*__start_routine)(void*)
    // 使用FFmpeg开始解封装
    pthread_create(&pid_prepare, nullptr, task_prepare, this);
}

void SuperPlayer::start() {
    // 一切准备就绪开始播放

    //
}
