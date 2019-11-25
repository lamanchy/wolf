# escape=`

FROM lamanchy/wolf_base

# prepare wolf app
RUN cp -rf /wolf_lib/examples/app /wolf_app && `
# configure release
    mkdir /wolf_app-build && `
    cd /wolf_app-build && `
    cmake -DWOLF_PATH=/wolf_lib -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/wolf_app/install/linux-docker -G "CodeBlocks - Unix Makefiles" /wolf_app && `
    # remove downloaded files to save space
    rm -rf wolf_lib/lib_source && `
# build release
    cmake --build . --target install -- -j 4


ENTRYPOINT ["/bin/bash", "-c"]
CMD "echo 'Initializing wolf app?' && `
     $ [ "$(ls -A /wolf_app)" ] && (echo "True"; cp -rf /wolf_lib/examples/app/* /wolf_app) || echo "False" && `
     cd /wolf_app-build && `
     cmake --build . --target install -- -j 4"

