#!/bin/bash
if [ "$#" -eq 1 ]; then
    basepath=$1
else
    basepath="$(cd "$(dirname "$0")"; pwd -P)"
fi

# if [[ "$(sudo docker images -q wolf 2> /dev/null)" == "" ]]; then
  sudo docker build -t wolf ${basepath}/linux;
# fi

sudo docker run --rm -v ${basepath}:/wolf wolf;

# docker created files are roots, well, it's hard to remove them then, you know
find ${basepath} -user root -exec sudo chown -R $USER: {} +
