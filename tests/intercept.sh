#!/bin/bash
touch *.c
qmake test.pro CONFIG+=grabtest && sudo make
sudo chmod 4755 grabtest
