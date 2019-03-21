FROM ubuntu:16.04
RUN apt update -y && \
    apt install git g++ python python-pip wget -y && \
    python -m pip install --upgrade pip && \
    python -m pip install cmake
# cmake installed with pip, because it is newer version

ENTRYPOINT ["/bin/bash", "/wolf/linux/build-wolf.sh"]
CMD ["docker"]