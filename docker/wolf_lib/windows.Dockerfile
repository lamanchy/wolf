# escape=`

FROM lamanchy/wolf_base

# configure release
RUN mkdir wolf_lib-build; `
    cd C:\wolf_lib-build; `
    cmake.exe -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=C:/wolf_lib/install/windows-docker -G 'Visual Studio 15 2017 Win64' C:\wolf_lib; `
    rm -r -fo wolf_lib\lib_source;

ENTRYPOINT ["powershell.exe", "-NoLogo", "-ExecutionPolicy", "Bypass"]
CMD "cd C:\wolf_lib-build; `
     cmake.exe --build . --target install --config Release"
