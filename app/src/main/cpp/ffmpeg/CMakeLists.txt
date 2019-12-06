# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
# 设置ffmpeg库所在路径的目录
set(distribution_DIR ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fno-rtti -fno-exceptions -Wall")
if (${ANDROID_PLATFORM_LEVEL} LESS 12)
    message(FATAL_ERROR "OpenGL 2 is not supported before API level 11 \
                      (currently using ${ANDROID_PLATFORM_LEVEL}).")
    return()
elseif (${ANDROID_PLATFORM_LEVEL} LESS 18)
    add_definitions("-DDYNAMIC_ES3")
    set(GL3STUB_SRC src/gl3stub.c)
    set(OPENGL_LIB GLESv2)
else ()
    set(OPENGL_LIB GLESv3)
endif (${ANDROID_PLATFORM_LEVEL} LESS 12)

include_directories(headers)
aux_source_directory(Common common)

add_library( # Sets the name of the library.
        Bian
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        ${GL3STUB_SRC}
        common/esShapes.c
        common/esTransform.c
        common/esShader.c
        common/esUtil.c
        test/VideoPlay.c
        test/VideoCompress.c
        test/VideoFilter.c
        test/MyVideoPlayer.cpp
        test/RegisterControl.cpp
        test/AudioFrameDataBean.cpp
        test/AudioPlayer.cpp
        test/VideoList.c
        test/List.c
        test/MutilThreadPlayer.cpp
        test/NewVideoPlayer.cpp
        test/OpenGLPlayer.cpp
        test/TestVideoYUV.c)


# 定义源码所在目录
aux_source_directory(video VideoPlayDir)
add_library( # Sets the name of the library.
        VideoPlay
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        ${VideoPlayDir}
        )

add_library(avcodec
        SHARED
        IMPORTED)
set_target_properties(avcodec
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libavcodec.so)


add_library(avdevice
        SHARED
        IMPORTED)
set_target_properties(avdevice
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libavdevice.so)

add_library(avfilter
        SHARED
        IMPORTED)
set_target_properties(avfilter
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libavfilter.so)

add_library(avformat
        SHARED
        IMPORTED)
set_target_properties(avformat
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libavformat.so)


add_library(avutil
        SHARED
        IMPORTED)
set_target_properties(avutil
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libavutil.so)


add_library(swresample
        SHARED
        IMPORTED)
set_target_properties(swresample
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libswresample.so)


add_library(swscale
        SHARED
        IMPORTED)
set_target_properties(swscale
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libswscale.so)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

find_library( # Sets the name of the path variable.
        android-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        android)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        Bian
        VideoPlay)


target_link_libraries( # Specifies the target library.
        VideoPlay
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        swresample
        swscale
        jnigraphics
        ${OPENGL_LIB}

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${android-lib}
        OpenSLES
        EGL
        m)