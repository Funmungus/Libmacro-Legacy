#!/bin/bash

fileName=tmp.sh

if [[ $# -gt 1 ]]
then
	fileName=${2}
fi

#cp template.sh ${fileName}

sed "s/#####/${1}/g" <template.sh >${fileName}

