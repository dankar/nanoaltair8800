#!/bin/sh

docker build . -t altair-daemon
docker run -v $PWD/..:/altair8800 altair-daemon
