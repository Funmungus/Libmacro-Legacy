#!/bin/bash
touch *.c
qmake test.pro CONFIG+="static debug" CONFIG+=hottest && make
qmake test.pro CONFIG+="static debug" CONFIG+=modstest && make
qmake test.pro CONFIG+="static debug" CONFIG+=stagetest && make
qmake test.pro CONFIG+="static debug" CONFIG+=dispatchtest && make
qmake test.pro CONFIG+="static debug" CONFIG+=dispatchstandardtest && make
qmake test.pro CONFIG+="static debug" CONFIG+=hotstagedtest && make
