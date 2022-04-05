cmake_minimum_required(VERSION 3.10.2)

project("player")

set(FFMPEG ${CMAKE_SOURCE_DIR}/ffmpeg) # ffmpeg path
set(RTMP ${CMAKE_SOURCE_DIR}/rtmp) # rtmp path

include_directories(${FFMPEG}/include)

# 批量导入源文件
file(GLOB SRC_FILES *.cpp)

link_directories(
        ${FFMPEG}/libs/${CMAKE_ANDROID_ARCH_ABI}
        ${RTMP}/libs/${CMAKE_ANDROID_ARCH_ABI}
)

add_library(
        native-lib
        SHARED
        ${SRC_FILES})

find_library(
        log-lib
        log)

target_link_libraries(
        native-lib
        ${log-lib}
        # 忽略顺序的方式，导入
        -Wl,--start-group
        avcodec avfilter avformat avutil swresample swscale
        -Wl,--end-group
        z # libz.so库，是FFmpeg需要用ndk的z库，FFMpeg需要额外支持  libz.so
        rtmp # rtmp
        android # ANativeWindow
        OpenSLES
)