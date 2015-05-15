#!/bin/bash
touch *.c
qmake test.pro CONFIG+=signaltest && make
qmake test.pro CONFIG+=standardtest && make
qmake test.pro CONFIG+=sendtest && make
./signaltest 2>>test.log
./standardtest 2>>test.log
# send is manual test
# ./sendtest
