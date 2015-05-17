#!/bin/bash
touch *.c
qmake test.pro CONFIG+=arraytest && make
qmake test.pro CONFIG+=maptest && make
