#!/bin/bash

set -e

ADB_EXE=/home/menya/Android/platform-tools/adb
CMAKE_EXE=/home/menya/Android/cmake/3.6.4111459/bin/cmake
ANDROID_NDK=/home/menya/Android/ndk-bundle
export ANDROID_NDK=$ANDROID_NDK

TARGET_ANDROID_ABI=(arm64-v8a)
#TARGET_ANDROID_ABI=(armeabi-v7a)
#TARGET_ANDROID_ABI=(armeabi-v7a arm64-v8a)

NDK_TOOLCHAIN_ABI=(aarch64-linux-android)
#NDK_TOOLCHAIN_ABI=(arm-linux-androideabi)
#NDK_TOOLCHAIN_ABI=(arm-linux-androideabi aarch64-linux-android)

for index in "${!TARGET_ANDROID_ABI[@]}"; do
    ABI=${TARGET_ANDROID_ABI[$index]}
    echo ""
    echo "-- Building for $ABI"
    mkdir -p "build/${ABI}"
    pushd "build/$ABI"
    ${CMAKE_EXE} \
        -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
        -DANDROID_ABI=${ABI} \
        -DNDK_LINK_LIB_ARCH=${NDK_TOOLCHAIN_ABI[$index]} \
        -DANDROID_PLATFORM=21 ../../ \
        -DCMAKE_BUILD_TYPE=DEBUG
    make -j 4
    [[ -f ./tester ]] \
      && ${ADB_EXE} shell su -c "rm -rf /data/local/tmp/tracker/${ABI}/tester" \
      && ${ADB_EXE} push tester "/data/local/tmp/tracker/${ABI}/tester"
    [[ -f ./injector ]] \
      && ${ADB_EXE} shell su -c "rm -rf /data/local/tmp/tracker/${ABI}/injector" \
      && ${ADB_EXE} push injector "/data/local/tmp/tracker/${ABI}/injector"
    [[ -f ./libagent.so ]] \
      && ${ADB_EXE} shell su -c "rm -rf /data/local/tmp/tracker/${ABI}/libagent.so" \
      && ${ADB_EXE} push libagent.so "/data/local/tmp/tracker/${ABI}/libagent.so"
    [[ -f ./libbypass_hacking.so ]] \
      && ${ADB_EXE} shell su -c "rm -rf /data/local/tmp/tracker/${ABI}/libbypass_hacking.so" \
      && ${ADB_EXE} push libbypass_hacking.so "/data/local/tmp/tracker/${ABI}/libbypass_hacking.so"
    popd
    echo "-- Building done"
    echo ""
done
