FROM ubuntu:16.04
RUN apt update -y && \
    apt install git g++ python3 python3-pip wget -y && \
    python3 -m pip install --upgrade pip && \
    python3 -m pip install cmake
# cmake installed with pip, because it is newer version

ENTRYPOINT ["/bin/bash", "-c"]
CMD ["/wolf/linux/build-wolf.sh docker"]