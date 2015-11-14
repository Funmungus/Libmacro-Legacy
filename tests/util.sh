#!/bin/bash
touch *.c
qmake test.pro CONFIG+="debug" $@ CONFIG+=arraytest && make
qmake test.pro CONFIG+="debug" $@ CONFIG+=maptest && make
