#!/bin/bash
touch *.c
qmake test.pro CONFIG+="static debug" CONFIG+=grabtest && make
