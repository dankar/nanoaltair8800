FROM ubuntu:20.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt update
RUN apt -y install make gcc git npm pkg-config \
           libczmq-dev libczmq4 nodejs npm
VOLUME /altair8800
WORKDIR /altair8800/software
CMD ../daemon-container/run.sh

