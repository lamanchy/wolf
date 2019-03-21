#!/bin/bash

basepath="$(dirname "$(cd "$(dirname "$0")"; pwd -P)")"


build_path_part=local
if [[ $# -ge 1 ]]
then
  build_path_part=$1
fi


for build_type in Debug Release
do
    python ${basepath}/install_libraries.py

    build_path=${basepath}/build/${build_path_part}/${build_type}

    mkdir -p ${build_path}
    cd ${build_path}

    cmake -DCMAKE_BUILD_TYPE=${build_type} -DCMAKE_INSTALL_PREFIX=${basepath}/bin -G "CodeBlocks - Unix Makefiles" ${basepath}

    cmake --build ${build_path} -- -j 4

#    python ${basepath}/tests/end_to_end_tests.py || exit 1

    cmake --build ${build_path} --target install -- -j 4
done
