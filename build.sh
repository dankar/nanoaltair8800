#!/bin/sh

if [ $# -eq 1 ]
then
	mkdir -p firmware
	docker build . --build-arg TARGET=$1 -t dankar/altair8800:build
	docker container create --name dankar-extract dankar/altair8800:build
	if [ $1 = "archive" ]
	then
		mkdir -p releases
		docker cp dankar-extract:/project/releases .
	else
		docker cp dankar-extract:/project/software/build/$1/nanoaltair.hex firmware/nanoaltair-$1.hex
	fi
	docker container rm dankar-extract > /dev/null 2>&1
else
	echo "You must supply a build target as parameter. (release or debug, perhaps)"
fi
