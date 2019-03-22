#!/bin/bash

basepath="$(cd "$(dirname "$0")"; pwd -P)"

if [[ "$(sudo docker images -q wolf 2> /dev/null)" == "" ]]; then
  sudo docker build -t wolf ${basepath}/linux;
fi

sudo docker run -v ${basepath}:/wolf -it wolf;

# docker created files are roots, well, it's hard to remove them then, you know
find ${basepath} -user root -exec sudo chown -R $USER: {} +
