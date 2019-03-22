FROM ubuntu:16.04
RUN apt update -y && \
    apt install git g++ python python-pip wget -y && \
    python -m pip install --upgrade pip && \
    python -m pip install cmake
# cmake installed with pip, because it is newer version

ENTRYPOINT ["/bin/bash", "-c"]
CMD ["/wolf/linux/build-wolf.sh docker"]