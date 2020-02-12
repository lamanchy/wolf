# Wolf library

This project contains everything you need to build a create and build a Wolf pipeline:

- Wolf core files
- fetching and compilation of dependencies
- Wolf common plugins
- definition of Docker images for build of this library / Wolf pipeline
- example Wolf pipeline, used to create new projects

To create new wolf pipeline, simply create empty folder and run within it:

`docker run --rm -v ${PWD}:C:\wolf wolf_win`

(available for linux as well)

To recompile any changes made, run the command above again.
