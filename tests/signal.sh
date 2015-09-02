#!/bin/bash
touch *.c
qmake test.pro CONFIG+="static debug" CONFIG+=signaltest && make
qmake test.pro CONFIG+="static debug" CONFIG+=standardtest && make
qmake test.pro CONFIG+="static debug" CONFIG+=sendtest && make
