#!/bin/bash

drs="${G}/funlibrary/include ${G}/funlibrary/tests ${G}/funlibrary/util ${G}/funlibrary/signal ${G}/funlibrary/hotkey ${G}/funlibrary/macro ${G}/funlibrary/doxygen"

(for derpy in ${drs}
do
	cd ${derpy} && git diff 
done)

(cd ${G}/funlibrary && git diff)

