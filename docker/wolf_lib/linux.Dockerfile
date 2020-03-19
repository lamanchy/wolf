# escape=`

FROM lamanchy/wolf_base

RUN mkdir wolf_lib-build && `
    cd wolf_lib-build && `
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/wolf_lib/install/linux-docker -G "CodeBlocks - Unix Makefiles" /wolf_lib

ENTRYPOINT ["/bin/bash", "-c"]
CMD ["cd wolf_lib-build; cmake --build . --target install -- -j 4"]
