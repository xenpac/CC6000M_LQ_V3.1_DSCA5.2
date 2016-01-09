#cp  ../../../../lib/lib_warriors/init.o ../init.o

ANCHOR=../../../../../../../../config_prj/other/anchor/bin/anchor.exe

mv FastLogoAp.map ./release/
mv ./${1} ./release/
cd ./release/
#echo --oformat elf32-tradlittlemips -nostdlib -T"../link.lds" -L../../../../../lib/lib_warriors -o ${1}  > bb
#echo init.o >> bb
#"find.exe" ./ -name "*.o" >> bb
#echo -leva_w -lcore_warriors -leva_w -lc -lm -lgcc >> bb
#cp ../init.o ./
#cat bb | xargs mipsel-linux-ld.exe
#cp init.o src/hal/warriors/init.o
#rm -rf init.o
#rm -rf ../init.o
#rm -rf bb
mipsel-linux-objcopy -O binary ${1} temp.bin
$ANCHOR -p FastLogoAp.bin temp.bin
#mipsel-linux-objdump -d FastLogoAp.elf > FastLogoAp.s
#$ANCHOR -i new.bin FastLogoAp.bin
#$ANCHOR -t warriors.img -f ./binary/img_header.dat -f new.bin

AS=mips-elf-as
CC=mips-elf-gcc
LD=mips-elf-ld
AR=mips-elf-ar
OD=mips-elf-objdump
OC=mips-elf-objcopy
GS=mips-elf-gasp
NM=mips-elf-nm
RD=mips-elf-readelf
STRIP=mips-elf-strip
SIZE=mips-elf-size
LIBTOOL=mips-elf-libtool

CAT=cat
LS=ls
XARGS=xargs
RM=rm
CP=cp
ECHO=echo
SED=sed
FIND=find
MKDIR=mkdir

MAKE=make

MKIMG=mkimg_mips
LZMA=lzma

$MKIMG FastLogoAp.bin FastLogoAp.img -i
$CP FastLogoAp.bin FastLogoAp.tmp
$RM -f FastLogoAp.tmp.lzma
$LZMA -z -k FastLogoAp.tmp
$RM -f FastLogoAp.tmp
$MKIMG FastLogoAp.tmp.lzma FastLogoAp_lzma.img -g
$RM -f FastLogoAp.tmp.lzma
$RM -f FastLogoAp.img

$RM -rf ../binary
$MKDIR ../binary
$CP FastLogoAp_lzma.img ../binary/

