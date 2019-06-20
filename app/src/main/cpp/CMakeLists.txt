# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI})
# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
include_directories(headers)
# 设置ffmpeg库所在路径的目录
set(distribution_DIR ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})

add_library( # Sets the name of the library.
        bian-lib
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        bian-lib.cpp
        src/NdkTest.c
        )

# 定义源码所在目录
#aux_source_directory(src/example DEMO)

add_library(libavcodec
        SHARED
        IMPORTED)
set_target_properties(libavcodec
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libavcodec.so)


add_library(libavdevice
        SHARED
        IMPORTED)
set_target_properties(libavdevice
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libavdevice.so)


add_library(libavfilter
        SHARED
        IMPORTED)
set_target_properties(libavfilter
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libavfilter.so)

add_library(libavformat
        SHARED
        IMPORTED)
set_target_properties(libavformat
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libavformat.so)


add_library(libavutil
        SHARED
        IMPORTED)
set_target_properties(libavutil
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libavutil.so)


add_library(libswresample
        SHARED
        IMPORTED)
set_target_properties(libswresample
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libswresample.so)


add_library(libswscale
        SHARED
        IMPORTED)
set_target_properties(libswscale
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
        bian-lib
        libavcodec
        libavdevice
        libavfilter
        libavformat
        libavutil
        libswresample
        libswscale
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${android-lib})