#!/bin/bash

basepath="$(cd "$(dirname "$0")"; pwd -P)"

if [ "$#" -eq 1 ]; then
    mount_path=$1
else
    mount_path=$basepath
fi

# if [[ "$(sudo docker images -q wolf 2> /dev/null)" == "" ]]; then
  sudo -S docker build -t wolf ${basepath}/linux;
# fi

sudo -S docker run --rm -v ${mount_path}:/wolf wolf;

# docker created files are roots, well, it's hard to remove them then, you know
find ${basepath} -user root -exec sudo chown -R $USER: {} +
