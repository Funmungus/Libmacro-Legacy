#!/bin/bash
touch *.c
qmake test.pro CONFIG+=hottest && make $@
qmake test.pro CONFIG+=modstest && make $@
qmake test.pro CONFIG+=stagetest && make $@
qmake test.pro CONFIG+=dispatchtest && make $@
qmake test.pro CONFIG+=dispatchstandardtest && make $@
qmake test.pro CONFIG+=hotstagedtest && make $@
./hottest 2>>test.log
./modstest 2>>test.log
./stagetest 2>>test.log
./dispatchtest 2>>test.log
./dispatchstandardtest 2>>test.log
./hotstagedtest 2>>test.log
