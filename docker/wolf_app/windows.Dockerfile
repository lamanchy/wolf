# escape=`

FROM lamanchy/wolf_base

# prepare wolf app
RUN cp -r -fo C:\wolf_lib\example_app C:\wolf; `
# configure release
    mkdir C:\wolf-build; `
    cd C:\wolf-build; `
    cmake.exe -DWOLF_PATH=C:/wolf_lib -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=C:/wolf/install/windows-docker -G 'Visual Studio 15 2017 Win64' C:\wolf; `
# build release
    cmake.exe --build . --target install --config Release


ENTRYPOINT ["powershell.exe", "-NoLogo", "-ExecutionPolicy", "Bypass"]
CMD "Write-Host('Initializing wolf app...'); `
     (((Get-ChildItem C:\wolf | Measure-Object).count -eq 0) -and (cp -r -fo C:\wolf_lib\example_app\* C:\wolf)); `
     cd C:\wolf-build; `
     cmake.exe -DWOLF_PATH=C:/wolf_lib -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=C:/wolf/install/windows-docker -G 'Visual Studio 15 2017 Win64' C:\wolf; `
     cmake.exe --build . --target install --config Release"

