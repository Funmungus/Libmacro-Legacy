#!/bin/bash
touch *.c
qmake test.pro CONFIG+=grabtest && make
