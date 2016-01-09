LZMA=lzma
MV=mv

MKIMG=mkimg_mips
#rm -rf ./binary
mkdir -p release
mv hwcfg_client.map ./release/
mv hwcfg_client.elf ./release/

cd ./release/


ANCHOR=../../../../../config_prj/other/anchor/bin/anchor.exe
ANCHOR2=../../../../../../config_prj/other/anchor/bin/anchor.exe
ANCHOR3=../../../../../../../config_prj/other/anchor/bin/anchor.exe

mipsel-linux-objcopy -O binary ${1} hwcfg_client.bin
mipsel-linux-objdump -d hwcfg_client.elf > hwcfg_client.s

echo "post build process completes"



