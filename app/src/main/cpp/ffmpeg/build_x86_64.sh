#!/bin/bash
export TMPDIR=D:/NativeCode/ffmpeg-3.4.6/Temp
NDK=D:/NDK
SYSROOT=$NDK/platforms/android-21/arch-x86_64
TOOLCHAIN=$NDK/toolchains/x86_64-4.9/prebuilt/windows-x86_64
CPU=x86_64
PREFIX=D:/NativeCode/ffmpeg-3.4.6/output/$CPU

ADDI_CFLAGS="-march=x86-64 -msse4.2 -mpopcnt -m64 -mtune=intel"

function build_one
{
./configure \
--prefix=$PREFIX \
--enable-shared \
--disable-static \
--enable-asm \
--enable-neon \
--disable-doc \
--disable-ffmpeg \
--disable-ffplay \
--disable-ffprobe \
--disable-ffserver \
--disable-doc \
--disable-symver \
--enable-pthreads \
--enable-small \
--enable-jni \
--enable-mediacodec \
--enable-runtime-cpudetect \
--cross-prefix=$TOOLCHAIN/bin/x86_64-linux-android- \
--target-os=android \
--arch=x86_64 \
--enable-cross-compile \
--sysroot=$SYSROOT \
--extra-cflags="$ADDI_CFLAGS" \
--extra-ldflags="$ADDI_LDFLAGS" \
$ADDITIONAL_CONFIGURE_FLAG
make clean
make
make install
}
build_one

read -p "Press any key to continue."