#!/bin/sh

## CB From https://doc.qt.io/qt-6/linux-deployment.html
## CB Use this shell script to deploy your application.
## CB Copy and rename this filename to {your-appname}.sh
## CB i.e. "chemcal.sh"
## CB Don't change internals.

appname=`basename $0 | sed s,\.sh$,,`

dirname=`dirname $0`
tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi
LD_LIBRARY_PATH=$dirname
export LD_LIBRARY_PATH
$dirname/$appname "$@"
