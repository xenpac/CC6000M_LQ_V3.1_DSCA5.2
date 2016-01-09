#############<-- Set all the variables -->##############
#####PART1 please don't change############
#!/bin/sh
CORENAME=$1
LIBDIR=$2
WORK_PATH=$(cd "$(dirname "$0")"; pwd)

####PART3 please don't change############
LIB_CORENAME=lib${CORENAME}.a
LIB_DEST_CORENAME=lib${DEST_CORENAME}.a
LIB_PATH=${WORK_PATH}/${CORENAME}/bin/Release
RELEASE_PATH=../../../../lib/lib_${LIBDIR}/
AP_MDL_CODE_DIR=../../core_ap_mdl
AP_MDL_CODE_PATH=../../core_ap_mdl/${CORENAME}
#############<-- copy lib to a common place -->##############
if [ ! -d "$RELEASE_PATH" ]; then
mkdir ${RELEASE_PATH}
fi
cp ${LIB_PATH}/${LIB_CORENAME} ${RELEASE_PATH}
sed -e 's/mips24k/COMPILER_SET/g' ${WORK_PATH}/${CORENAME}/${CORENAME}.cbp>temp.cbp
sed -e 's/Add option="-EL -march=mips32r2.*/Add option="COMPILER_OPTION1" \/>/g' temp.cbp>temp1.cbp
if [ ! -d "$AP_MDL_CODE_DIR" ]; then
mkdir ${AP_MDL_CODE_DIR}
fi
if [ ! -d "$AP_MDL_CODE_PATH" ]; then
mkdir ${AP_MDL_CODE_PATH}
fi
cp -r -f temp1.cbp ${AP_MDL_CODE_PATH}/${CORENAME}.cbp
rm temp.cbp
rm temp1.cbp





