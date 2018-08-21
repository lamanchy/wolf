#!/bin/bash

cd "$(dirname "$0")"

install_lib () {
    rm -rf build
    mkdir build
    cd build

    echo "installing $1"

    cd ../submodules/$1
    git clean -fd
    cd -

    cmake ../submodules/$1 -DCMAKE_INSTALL_PREFIX=.. "${@:2}"
    cmake --build . --target install
    cd ..
    rm -rf build
}

echo "BOOST HAS TO BE INSTALLED MANUALLY, boost headers in include/boost/....h"
echo "json library from https://github.com/nlohmann/json/releases"
sleep 3

#install_lib re2
#install_lib rapidjson
#install_lib taojson
#install_lib zlib
#install_lib cxxopts
#install_lib stxxl
#install_lib librdkafka
#install_lib cppkafka -DCPPKAFKA_BUILD_SHARED=OFF -DCPPKAFKA_RDKAFKA_STATIC_LIB=OFF -DRDKAFKA_ROOT_DIR=../..

mkdir -p include
cp -r submodules/asio/asio/include/* include
cp -r submodules/gzip-hpp/include/* include
cp -r submodules/date/include/* include
