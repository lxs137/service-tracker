#!/bin/bash

fswatch --one-per-batch -o -r \
    --exclude 'tracker-native/cmake-build-debug-ndk' --exclude 'tracker-native/libs' ./tracker-native | while read f; 
    do rsync -ra \
        --exclude 'build' \
        --exclude 'cmake-build-debug-ndk' \
        --exclude 'libs' \
        --progress --delete tracker-native/ menya@menya-lab:/home/menya/testing/service-tracker/tracker-native;
    sleep 2;
done

