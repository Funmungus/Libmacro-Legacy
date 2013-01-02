#!/bin/bash

drs="${G}/funlibrary/include ${G}/funlibrary/tests ${G}/funlibrary/util ${G}/funlibrary/signal ${G}/funlibrary/hotkey ${G}/funlibrary/macro ${G}/funlibrary/doxygen"

(for derpy in ${drs}
do
	cd ${derpy} && gitk --all 
done)

(cd ${G}/funlibrary && gitk --all)

