import os
import shutil
import subprocess
import sys


def is_linux():
    return sys.platform == "linux" or sys.platform == "linux2"


def is_win():
    return not is_linux()


cmake = r"C:\Program Files\JetBrains\CLion 2018.3\bin\cmake\win\bin\cmake.exe"
if is_linux():
    cmake = "cmake"

compiler = "Visual Studio 15 2017 Win64"
if is_linux():
    compiler = "CodeBlocks - Unix Makefiles"

boost_version = "boost_1_69_0"

BASE_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'lib')

if not os.path.exists(BASE_DIR):
    os.mkdir(BASE_DIR)


def get_libs():
    os.chdir(BASE_DIR)

    libs = [
        dict(name="re2", git="https://github.com/google/re2.git", tag="2019-03-01"),
        dict(name="cppkafka", git="https://github.com/mfontanini/cppkafka.git", tag="v0.3.1"),
        dict(name="librdkafka", git="https://github.com/edenhill/librdkafka.git", tag="v0.11.6"),
        dict(name="asio", git="https://github.com/chriskohlhoff/asio.git", tag="asio-1-12-2"),
        dict(name="taojson", git="https://github.com/taocpp/json.git", tag="1.0.0-beta.11"),
        dict(name="stxxl", git="https://github.com/stxxl/stxxl.git", tag="1.4.1"),
        dict(name="zlib", git="https://github.com/madler/zlib.git", tag="v1.2.11"),
        dict(name="gzip-hpp", git="https://github.com/mapbox/gzip-hpp.git", tag="v0.1.0"),
        dict(name="date", git="https://github.com/HowardHinnant/date.git", tag="v2.4.1"),
        dict(name="cxxopts", git="https://github.com/jarro2783/cxxopts.git"),  # needs master
    ]

    if not os.path.exists("submodules"):
        os.mkdir("submodules")
    else:
        return

    os.chdir("submodules")
    for lib in libs:
        subprocess.call(["git", "clone", lib["git"], lib["name"]])
        os.chdir(lib["name"])
        if "tag" in lib:
            subprocess.call(["git", "checkout", lib["tag"]])
        os.chdir("..")

    if is_linux():
        subprocess.call(["wget", "https://dl.bintray.com/boostorg/release/1.69.0/source/%s.tar.gz" % boost_version])
        subprocess.call(["tar", "-xf", "%s.tar.gz" % boost_version])
    else:
        raise NotImplemented("fuck fuck")

    os.chdir(BASE_DIR)


for build_type in ["Debug", "Release"]:
    os.chdir(BASE_DIR)
    build_path = os.path.join(BASE_DIR, "build")
    target_path = os.path.join(BASE_DIR, build_type)
    if os.path.exists(target_path):
        continue

    get_libs()
    os.mkdir(target_path)

    include_path = os.path.join(target_path, "include")
    if not os.path.exists(include_path):
        os.mkdir(include_path)


    def copy_headers(name, extend=""):
        print "copying %s header files" % name
        lib_path = os.path.join("submodules", name)
        for item in os.listdir(lib_path):
            s = os.path.join(lib_path, item)
            d = os.path.join(os.path.join(include_path, extend), item)
            if os.path.isdir(s):
                try:
                    shutil.rmtree(d)
                except OSError:
                    pass
                shutil.copytree(s, d)
            else:
                shutil.copy2(s, d)


    def install_lib(name, *args):
        lib_path = os.path.join(BASE_DIR, "submodules", name)
        try:
            shutil.rmtree(build_path)
        except OSError:
            pass

        print "installing " + name

        os.mkdir(build_path)
        os.chdir(build_path)

        make = r"C:/Program_Files/mingw-w64/x86_64-8.1.0-posix-sjlj-rt_v6-rev0/mingw64/bin/mingw32-make.exe"
        c_compiler = r"C:/Program_Files/mingw-w64/x86_64-8.1.0-posix-sjlj-rt_v6-rev0/mingw64/bin/gcc.exe"
        cpp_compiler = r"C:/Program_Files/mingw-w64/x86_64-8.1.0-posix-sjlj-rt_v6-rev0/mingw64/bin/g++.exe"

        # subprocess.call(["set", r"PATH=C:\Program_Files\mingw-w64\x86_64-8.1.0-posix-sjlj-rt_v6-rev0\mingw64\bin;%PATH%"])

        subprocess.call([
                            cmake,
                            "-DCMAKE_INSTALL_PREFIX=" + target_path,
                            '-DCMAKE_CXX_FLAGS_RELEASE=/MT /O2 /DNDEBUG' if is_win() else '',
                            '-DCMAKE_C_FLAGS_RELEASE=/MT /O2 /DNDEBUG' if is_win() else '',
                            '-DCMAKE_CXX_FLAGS_DEBUG=/MTd' if is_win() else '',
                            '-DCMAKE_C_FLAGS_DEBUG=/MTd' if is_win() else '',
                            '-DCMAKE_BUILD_TYPE=' + build_type,
                            # "-DCMAKE_C_COMPILER=" + c_compiler,
                            # "-DCMAKE_CXX_COMPILER=" + cpp_compiler,
                            # "-DCMAKE_MAKE_PROGRAM=" + make,
                            "-G", compiler,
                            lib_path,
                        ] + list(args))

        subprocess.call([cmake, "--build", build_path, "--target", "install", "--config", build_type, "--", "-j", "4"])

        os.chdir(BASE_DIR)
        shutil.rmtree(build_path)


    copy_headers(os.path.join("asio", "asio", "include"))
    copy_headers(os.path.join("gzip-hpp", "include"))
    copy_headers(os.path.join("date", "include"))
    copy_headers(os.path.join(boost_version, "boost"), "boost")

    install_lib("re2",
                "-DRE2_BUILD_TESTING=OFF")
    install_lib("taojson",
                "-DTAOCPP_JSON_BUILD_TESTS=OFF",
                "-DTAOCPP_JSON_BUILD_EXAMPLES=OFF")
    install_lib("zlib")
    install_lib("cxxopts")
    install_lib("stxxl")
    install_lib("librdkafka",
                "-DRDKAFKA_BUILD_STATIC=ON",
                "-DRDKAFKA_BUILD_EXAMPLES=OFF",
                "-DRDKAFKA_BUILD_TESTS=OFF"
                )
    install_lib("cppkafka",
                "-DRDKAFKA_ROOT_DIR=" + target_path,
                "-DBOOST_ROOT=" + BASE_DIR,
                "-DCPPKAFKA_RDKAFKA_STATIC_LIB=ON",
                "-DCPPKAFKA_BUILD_SHARED=OFF",
                "-DCPPKAFKA_DISABLE_TESTS=ON",
                )

try:
    shutil.rmtree("submodules")
except OSError:
    pass
