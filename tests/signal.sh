#!/bin/bash
touch *.c
qmake test.pro CONFIG+="debug" $@ CONFIG+=signaltest && make
qmake test.pro CONFIG+="debug" $@ CONFIG+=standardtest && make
qmake test.pro CONFIG+="debug" $@ CONFIG+=sendtest && make
