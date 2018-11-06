import os
import shutil
import subprocess

import errno

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
os.chdir(BASE_DIR)
build_path = os.path.join(BASE_DIR, "build")

include_path = os.path.join(BASE_DIR, "include")


def install_lib(name, *args):
    lib_path = os.path.join(BASE_DIR, "submodules",  name)
    try:
        shutil.rmtree(build_path)
    except WindowsError:
        pass

    os.mkdir(build_path)

    print "installing " + name

    os.chdir(lib_path)

    subprocess.call(["git", "clean", "-fd"])

    os.chdir(build_path)

    # cmake = r"C:\Program Files\CMake\bin\cmake.exe"
    cmake = r"C:\Program Files\JetBrains\CLion 2018.2.1\bin\cmake\win\bin\cmake.exe"
    make = r"C:/Program_Files/mingw-w64/x86_64-8.1.0-posix-sjlj-rt_v6-rev0/mingw64/bin/mingw32-make.exe"
    c_compiler = r"C:/Program_Files/mingw-w64/x86_64-8.1.0-posix-sjlj-rt_v6-rev0/mingw64/bin/gcc.exe"
    cpp_compiler = r"C:/Program_Files/mingw-w64/x86_64-8.1.0-posix-sjlj-rt_v6-rev0/mingw64/bin/g++.exe"

    # subprocess.call(["set", r"PATH=C:\Program_Files\mingw-w64\x86_64-8.1.0-posix-sjlj-rt_v6-rev0\mingw64\bin;%PATH%"])
    subprocess.call([
                        cmake,
                        "-DCMAKE_INSTALL_PREFIX=" + BASE_DIR,
                        '-DCMAKE_CXX_FLAGS_RELEASE=/MT /O2 /DNDEBUG',
                        '-DCMAKE_C_FLAGS_RELEASE=/MT /O2 /DNDEBUG',
                        # "-DCMAKE_C_COMPILER=" + c_compiler,
                        # "-DCMAKE_CXX_COMPILER=" + cpp_compiler,
                        # "-DCMAKE_MAKE_PROGRAM=" + make,
                        # "-G", "MinGW Makefiles",
                        "-G", "Visual Studio 15 2017 Win64",
                        lib_path,
                    ] + list(args))

    subprocess.call([cmake, "--build", build_path, "--target", "install", "--config", "Release"])

    os.chdir(BASE_DIR)
    shutil.rmtree(build_path)



print "BOOST HAS TO BE INSTALLED MANUALLY, boost headers in include/boost/....h"
# sleep(3)

install_lib("re2")
install_lib("taojson")
install_lib("zlib")
install_lib("cxxopts")
install_lib("stxxl")
install_lib("librdkafka",
            "-DRDKAFKA_BUILD_STATIC=ON",
            )
install_lib("cppkafka",
            "-DRDKAFKA_ROOT_DIR=" + BASE_DIR,
            "-DBOOST_ROOT=" + BASE_DIR,
            "-DCPPKAFKA_RDKAFKA_STATIC_LIB=ON",
            "-DCPPKAFKA_BUILD_SHARED=OFF",
            "-DCPPKAFKA_DISABLE_TESTS=ON",
            )

try:
    os.makedirs(include_path)
except OSError as exc:  # Python >2.5
    if exc.errno == errno.EEXIST and os.path.isdir(include_path):
        pass
    else:
        raise


def copy_headers(name):
    lib_path = os.path.join("submodules", name)
    for item in os.listdir(lib_path):
        s = os.path.join(lib_path, item)
        d = os.path.join(include_path, item)
        if os.path.isdir(s):
            shutil.copytree(s, d)
        else:
            shutil.copy2(s, d)


# copy_headers(os.path.join("asio", "asio", "include"))
# copy_headers(os.path.join("gzip-hpp", "include"))
# copy_headers(os.path.join("date", "include"))
