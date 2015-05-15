#!/bin/bash
touch *.c
qmake test.pro CONFIG+=arraytest && make
qmake test.pro CONFIG+=maptest && make
./arraytest 2>>test.log
./maptest 2>>test.log
