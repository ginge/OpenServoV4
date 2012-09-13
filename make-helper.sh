#!/bin/bash

export LIB_MAPLE_HOME=/home/baz/arm/maple/libmaple
export PATH=$PATH:~/arm/maple/libmaple/arm/bin
SCRIPT=`readlink -f $0`
SCRIPTDIR=`dirname $SCRIPT`
USER_MODULES="$SCRIPTDIR/libfoo $SCRIPTDIR" make -f $LIB_MAPLE_HOME/Makefile $@
