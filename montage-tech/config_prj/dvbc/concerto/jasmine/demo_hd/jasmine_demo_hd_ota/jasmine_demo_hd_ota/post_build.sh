LZMA=lzma
MV=mv

MKIMG=mkimg_mips
CP=cp
rm -rf ./binary

mv jasmine_demo_hd_ota.map ./release/
mv ./${1} ./release/
cd ./release/
ANCHOR=../../../../../../../other/anchor/bin/anchor.exe

mipsel-linux-objcopy -O binary ${1} jasmine_demo_hd.bin
$ANCHOR -p jasmine_demo_hd_ota.bin jasmine_demo_hd.bin
rm -f jasmine_demo_hd.bin
#mipsel-linux-objdump -d jasmine_demo_hd_ota.elf > jasmine_demo_hd_ota.s
$ANCHOR -i jasmine_demo_hd_ota.img jasmine_demo_hd_ota.bin 0x80008000 0x80008000

  cp jasmine_demo_hd_ota.bin jasmine_demo_hd_ota.tmp
	rm -f jasmine_demo_hd_ota.tmp.lzma
	lzma -z -k jasmine_demo_hd_ota.tmp
	$MKIMG jasmine_demo_hd_ota.tmp.lzma jasmine_demo_hd_ota_lzma.img -g
	rm -f jasmine_demo_hd_ota.tmp.lzma

echo "post build process completes"