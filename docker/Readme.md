# Wolf docker images

## Wolf base image

- dependencies to build wolf
    - compiler
    - cmake
    - python for tests
    - git to get libraries
- build with:
    - `docker build -m 2GB -t wolf_base -f .\docker\wolf_base\windows.Dockerfile .`

- used for build of wolf library
- `build` prepares environment, also compiles all libraries
- `run` builds wolf library, installs it to wolf/install

## Wolf lib image

- used to build wolf library
- contains built wolf's libraries
- requires wolf_base
- build with:
    - `docker build -t wolf_lib -f .\docker\wolf_lib\windows.Dockerfile .`
- run (compile wolf) with:
    - `docker run --rm -v ${PWD}:C:\wolf_lib wolf_lib`

## Wolf app image

- used to build wolf app
- when run over empty directory, it initializes it
- contains built wolf lib with all dependencies
- build with:
    - `docker build -t wolf -f .\docker\wolf_app\windows.Dockerfile .`
- run (compile wolf app) with:
    - `docker run --rm -v ${PWD}:C:\wolf wolf`
    - where `${PWD}` is directory with wolf app
        - empty directory, when creating new app

## Linux notes

- all images can be run under linux as well
- just change paths and run `linux.Dockerfile`