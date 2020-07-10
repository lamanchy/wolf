# Wolf library

## What is Wolf?
Wolf is a stream processing tool, designed to replace Logstash within Y Soft Corporation, but
the core is kept public and open-source. 

## How to create processing pipeline?
Simplest way is by using prebuild docker image, create empty directory and run from within: 
```
docker run --rm -v ${PWD}:C:\wolf lamanchy/wolf_win
docker run --rm -v ${PWD}:/wolf lamanchy/wolf_linux
```
The first run creates empty example pipeline, subsequent runs of the same command compiles
any changes made to the pipeline.

If you need, you can also compile newly created project using CMake and a compiler, the provided
CMake files not only compile Wolf library, but also download and compile all necessary dependencies.

## Repository structure

- `.github`
    - contains definition of github workflows which automatically build docker images
- `cmake`
    - all cmake files
- `docker`
    - definition of docker images
    - `wolf_base`
        - common image, describing system requirements
    - `wolf_lib`
        - image used to build static library out of wolf lib repository
    - `wolf_app`
        - image containing prebuilt wolf library, used to create wolf app and compile it
- `example_app`
    - source files for example application, these are copied when running `wolf_app` image over
    directory, but can be copied manually as well
- `include`
    - header files
    - `base`
        - core Wolf files, pipeline, plugin, etc.
    - `extras`
        - helper functions, classes
    - `libs`
        - third-party libs without github (cannot be downloaded before build)
    - `plugins`
        - Wolf plugins used in pipeline
- `src`
    - source files