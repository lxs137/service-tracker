#!/bin/bash

fswatch -o -r -l 5 \
    --exclude 'tracker-native/cmake-build-debug-ndk' ./tracker-native | while read f; 
    do rsync -ra \
        --exclude 'build' \
        --exclude 'cmake-build-debug-ndk' \
        --progress --delete tracker-native/ menya@menya-lab:/home/menya/testing/service-tracker/tracker-native;
done

