# escape=`

FROM lamanchy/wolf_base

# prepare wolf app
RUN cp -r -fo C:\wolf_lib\examples\app C:\wolf_app; `
# configure release
    mkdir C:\wolf_app-build; `
    cd C:\wolf_app-build; `
    cmake.exe -DWOLF_PATH=C:/wolf_lib -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=C:/wolf_app/install/windows-docker -G 'Visual Studio 15 2017 Win64' C:\wolf_app; `
    # remove downloaded files to save space
    rm -r -fo wolf_lib\lib_source; `
# build release
    cmake.exe --build . --target install --config Release


ENTRYPOINT ["powershell.exe", "-NoLogo", "-ExecutionPolicy", "Bypass"]
CMD "Write-Host('Initializing wolf app...'); `
     (((Get-ChildItem C:\wolf_app | Measure-Object).count -eq 0) -and (cp -r -fo C:\wolf_lib\examples\app\* C:\wolf_app)); `
     cd C:\wolf_app-build; `
     cmake.exe -DWOLF_PATH=C:/wolf_lib -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=C:/wolf_app/install/windows-docker -G 'Visual Studio 15 2017 Win64' C:\wolf_app; `
     cmake.exe --build . --target install --config Release"

