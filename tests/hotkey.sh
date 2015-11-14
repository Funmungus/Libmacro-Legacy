#!/bin/bash
touch *.c
qmake test.pro CONFIG+="debug" $@ CONFIG+=hottest && make
qmake test.pro CONFIG+="debug" $@ CONFIG+=modstest && make
qmake test.pro CONFIG+="debug" $@ CONFIG+=stagetest && make
qmake test.pro CONFIG+="debug" $@ CONFIG+=dispatchtest && make
qmake test.pro CONFIG+="debug" $@ CONFIG+=dispatchstandardtest && make
qmake test.pro CONFIG+="debug" $@ CONFIG+=hotstagedtest && make
