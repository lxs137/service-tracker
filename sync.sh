#!/bin/bash

fswatch -o -r -l 3 \
    --exclude 'tracker/.externalNativeBuild' \
    --exclude 'tracker/build' \
    --exclude 'tracker/.gradle' ./tracker | while read f; 
    do rsync -ra \
        --exclude '.externalNativeBuild' \
        --exclude 'build' \
        --exclude '.gradle' \
        --progress --delete tracker/ menya@menya-lab:/home/menya/testing/service-tracker/tracker;
done

