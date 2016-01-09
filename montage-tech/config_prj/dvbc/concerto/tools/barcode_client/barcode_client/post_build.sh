LZMA=lzma
MV=mv

MKIMG=mkimg_mips
#rm -rf ./binary
mkdir -p release
mv barcode_client.map ./release/
mv barcode_client.elf ./release/

cd ./release/


ANCHOR=../../../../../config_prj/other/anchor/bin/anchor.exe
ANCHOR2=../../../../../../config_prj/other/anchor/bin/anchor.exe
ANCHOR3=../../../../../../../config_prj/other/anchor/bin/anchor.exe

mipsel-linux-objcopy -O binary ${1} barcode_client.bin
rm -rf barcode_client.bin.lzma
$LZMA -z -k barcode_client.bin

echo "post build process completes"



