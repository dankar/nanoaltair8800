FROM ubuntu:20.04
RUN apt update
RUN apt -y install make gcc gcc-arm-none-eabi dfu-util git zip
ENV BUILD_TARGET=debug
VOLUME /altair8800
WORKDIR /altair8800/software
CMD make $BUILD_TARGET -j8

