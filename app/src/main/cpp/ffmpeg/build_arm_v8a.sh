#!/bin/bash
export TMPDIR=D:/NativeCode/ffmpeg-3.4.6/temp
NDK=D:/NDK
SYSROOT=$NDK/platforms/android-21/arch-arm64
TOOLCHAIN=$NDK/toolchains/aarch64-linux-android-4.9/prebuilt/windows-x86_64
CPU=arm64-v8a
PREFIX=D:/NativeCode/ffmpeg-3.4.6/output/$CPU

ADDI_CFLAGS="-march=armv8-a -I D:/NativeCode/ffmpeg-3.4.6/x264/arm64/include"
ADDI_LDFLAGS="-L$TOOLCHAIN/sysroot/usr/lib -L D:/NativeCode/ffmpeg-3.4.6/x264/arm64/lib"
ADDITIONAL_CONFIGURE_FLAG="--arch=aarch64"

function build_one
{
./configure \
--prefix=$PREFIX \
--disable-doc \
--enable-shared \
--disable-static \
--enable-mediacodec \
--enable-decoder=h264_mediacodec \
--disable-yasm \
--disable-symver \
--enable-gpl \
--disable-ffmpeg \
--disable-ffplay \
--disable-ffprobe \
--disable-ffserver \
--disable-doc \
--disable-symver \
--enable-libx264
--cross-prefix=$TOOLCHAIN/bin/aarch64-linux-android- \
--target-os=android \
--arch=arm64-v8a \
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