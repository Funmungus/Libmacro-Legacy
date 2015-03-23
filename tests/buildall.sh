#!/bin/bash
qmake test.pro CONFIG+=arraytest && make $@
qmake test.pro CONFIG+=maptest && make $@
qmake test.pro CONFIG+=signaltest && make $@
qmake test.pro CONFIG+=standardtest && make $@
qmake test.pro CONFIG+=sendtest && make $@
qmake test.pro CONFIG+=hottest && make $@
qmake test.pro CONFIG+=modstest && make $@
qmake test.pro CONFIG+=dispatchtest && make $@
qmake test.pro CONFIG+=dispatchstandardtest && make $@
