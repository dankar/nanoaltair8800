#!/bin/sh

if [ $# -eq 1 ]
then
	mkdir -p firmware
	docker build . -t dankar/altair8800
	docker run -e BUILD_TARGET=$1 --user $(id -u):$(id -g) -v $PWD:/altair8800 dankar/altair8800
else
	echo "You must supply a build target as parameter. (all, create_release)"
fi
