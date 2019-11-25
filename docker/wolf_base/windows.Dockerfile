# escape=`

# Use the latest Windows Server Core image with .NET Framework 4.8.
FROM mcr.microsoft.com/windows/servercore:ltsc2019

# Restore the default Windows shell for correct batch processing.
SHELL ["powershell.exe", "-NoLogo", "-ExecutionPolicy", "Bypass", "-Command"]

# install choco
RUN (iex ((new-object net.webclient).DownloadString('https://chocolatey.org/install.ps1'))) >$null 2>&1

RUN choco install git -y --no-progress; `
    choco install python -y --no-progress; `
    choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System' -y --no-progress; `
    choco install visualcpp-build-tools --version 15.0.26228.20170424 -y --no-progress; `
    Write-Host 'Waiting for Visual C++ Build Tools to finish'; `
    Wait-Process -Name vs_installer;

COPY . wolf_lib