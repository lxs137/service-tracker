#!/bin/bash

CMAKE_EXE=/home/menya/Android/cmake/3.6.4111459/bin/cmake
ANDROID_NDK=/home/menya/Android/ndk-bundle
export ANDROID_NDK=$ANDROID_NDK


TARGET_ANDROID_ABI=(armeabi-v7a arm64-v8a)
NDK_TOOLCHAIN_ABI=(arm-linux-androideabi aarch64-linux-android)


for index in "${!TARGET_ANDROID_ABI[@]}"; do
    ABI=${TARGET_ANDROID_ABI[$index]}
    echo ""
    echo "-- Building for $ABI"
    mkdir -p "build/${ABI}"
    pushd "build/$ABI"
    ${CMAKE_EXE} \
        -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
        -DANDROID_ABI=$ABI \
        -DNDK_LINK_LIB_ARCH=${NDK_TOOLCHAIN_ABI[$index]} \
        -DANDROID_PLATFORM=21 ../../
    make -j 4
    popd
    echo "-- Building done"
    echo ""
done

