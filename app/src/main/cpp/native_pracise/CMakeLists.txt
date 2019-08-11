# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
#定义头文件目录
include_directories(head)
aux_source_directory(./src SourceCode)
add_library(NdkTest
            SHARED
          ${SourceCode})


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
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${android-lib})