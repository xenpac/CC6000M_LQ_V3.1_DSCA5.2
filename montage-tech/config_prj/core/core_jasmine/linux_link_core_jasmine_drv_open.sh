#############<-- Set all the variables -->##############
#####PART1 please don't change############
#!/bin/sh
CORENAME=$1
CHIP=concerto
WORK_PATH=$(cd "$(dirname "$0")"; pwd)
#####PART2 please change############
if [ -z "${DEST_CORENAME}" ]; then
DEST_CORENAME=core_jasmine_drv_open
fi

####PART3 please don't change############
LIB_CORENAME=lib${CORENAME}.a
LIB_DEST_CORENAME=lib${DEST_CORENAME}.a
LIB_PATH=${WORK_PATH}/${CORENAME}/bin/Release
RELEASE_PATH=../../../../lib/lib_${CHIP}
#############<-- copy lib to a common place -->##############
cp ${LIB_PATH}/${LIB_CORENAME} $RELEASE_PATH





