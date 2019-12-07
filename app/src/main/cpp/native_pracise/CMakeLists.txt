# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# 设置libjpeg库所在路径的目录
set(distribution_DIR ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -Wall")
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

#定义头文件目录
include_directories(head)
add_library(NdkTest
        SHARED
        ${GL3STUB_SRC}
        src/common/TimeUtil.c
        src/common/esUtil.c
        src/common/esShader.c
        src/common/esTransform.c
        src/common/esShapes.c
        src/MainControl.cpp
        src/FirstOpenGLDrawer.cpp
        src/SurfaceViewDrawer.cpp
        src/BaseOpenGlDrawer.cpp
        src/SecondViewDrawer.cpp
        src/OffScreenDrawer.cpp
        src/PointDrawer.cpp
        src/AssetReader.c
        src/InstancingDrawer.cpp
        src/BitmapDrawer.cpp
        src/CubeDrawer.cpp
        src/NewCuberDrawer.cpp)

add_library(jpeg
        SHARED
        IMPORTED)
set_target_properties(jpeg
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libjpeg.so)

add_library(turbojpeg
        SHARED
        IMPORTED)
set_target_properties(turbojpeg
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libturbojpeg.so)

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

target_link_libraries( # Specifies the target library.
        NdkTest
        jpeg
        turbojpeg
        jnigraphics
        ${OPENGL_LIB}
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${android-lib}
        EGL
        m)