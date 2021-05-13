#!/bin/sh

docker build . -t altair-daemon
docker run -d -v $PWD/..:/altair8800 --user $(id -u):$(id -g) -p 3465:3465 altair-daemon
