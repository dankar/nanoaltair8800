FROM ubuntu:20.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt update
RUN apt -y install cmake git zip gcc gcc-arm-none-eabi make
ENV BUILD_TARGET=all
VOLUME /altair8800
WORKDIR /altair8800
CMD mkdir -p build && cd build && rm -rf * && cmake .. && make $BUILD_TARGET -j

