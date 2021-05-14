#!/bin/sh

docker build . -t altair-daemon
docker run -v $PWD/..:/altair8800 -p 3465:3465 altair-daemon
