FROM ubuntu:20.04
RUN apt update
RUN apt -y install make gcc gcc-arm-none-eabi dfu-util git zip
ARG TARGET=debug
ENV TARGET $TARGET
COPY . /project
RUN cd /project/software && make $TARGET -j8

