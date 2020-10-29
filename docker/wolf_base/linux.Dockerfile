# escape=`

FROM ubuntu:16.04
RUN apt update -y && `
    apt install git g++ python3 python3-pip wget -y && `
    python3 -m pip install --upgrade pip && `
# cmake installed with pip, because it is newer version
    python3 -m pip install cmake

COPY . wolf_lib