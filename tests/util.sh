#!/bin/bash
touch *.c
qmake test.pro CONFIG+="static debug" CONFIG+=arraytest && make
qmake test.pro CONFIG+="static debug" CONFIG+=maptest && make
