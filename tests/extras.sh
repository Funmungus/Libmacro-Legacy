#!/bin/bash
touch *.c
qmake test.pro CONFIG+="debug" $@ CONFIG+=crypttest && make
qmake test.pro CONFIG+="debug" $@ CONFIG+=cmdtest && make
