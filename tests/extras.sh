#!/bin/bash
touch *.c
qmake test.pro CONFIG+=crypttest && make
