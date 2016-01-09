LZMA=lzma
MV=mv

MKIMG=mkimg_mips
rm -rf ./binary

mv jasmine_demo_hd.map ./release/
mv ./${1} ./release/
cd ./release/


ANCHOR=../../../../../../../../prj/dvbc/jasmine_demo/tools/anchor.exe
ANCHOR2=../../../../../../../../../prj/dvbc/jasmine_demo/tools/anchor.exe
ANCHOR3=../../../../../../../../../../prj/dvbc/jasmine_demo/tools/anchor.exe


mipsel-linux-objcopy -O binary ${1} jasmine_demo_hd.bin
$ANCHOR -p concerto.bin jasmine_demo_hd.bin
rm -f jasmine_demo_hd.bin

#mipsel-linux-objdump -d jasmine_demo_hd.elf > concerto.s
$ANCHOR -i concerto.img concerto.bin 0x80100000 0x80100000

      cp concerto.bin concerto.tmp
	rm -f concerto.tmp.lzma
	lzma -z -k concerto.tmp
	#rm -f concerto.tmp
	$MKIMG concerto.tmp.lzma concerto_lzma.img -g
	rm -f concerto.tmp.lzma

$MV -f concerto.bin ./binary/
$MV -f concerto_lzma.img ./binary/
cd binary/ota_binary
$ANCHOR3 -b ./ota_flash.cfg ./../bootloader.bin
cd ../
echo  "anchor maincode.bin ............"
$ANCHOR2 -b ./maincode.cfg ./maincode.bin
echo  "anchor app_all.bin ............"
$ANCHOR2 -b ./app_all.cfg ./variation/app_all.bin
echo  "anchor flash.bin ............"
$ANCHOR2 -b ./flash.cfg ./flash.tmp.bin
$ANCHOR2 -p flash.bin flash.tmp.bin
rm -f flash.tmp.bin
cp ./flash.bin ../flash.bin
cd ../

echo  "backup jasmine_demo_hd.elf & jasmine_demo_hd.map ............"
cp ./jasmine_demo_hd.elf ./binary
cp ./jasmine_demo_hd.map ./binary

cd ../
mv ../../../../../../../src/kware/cas/cas_adapter_bak.h ../../../../../../../src/kware/cas/cas_adapter.h 2>null
mv ../../../../../../../src/kware/ads/ads_adapter_bak.h ../../../../../../../src/kware/ads/ads_adapter.h 2>null
mv ../../../../../../../inc/kware/cas_ware_bak.h ../../../../../../../inc/kware/cas_ware.h 2>null
mv ../../../../../../../inc/kware/ads_ware_bak.h ../../../../../../../inc/kware/ads_ware.h 2>null
mv ../../../../../../../inc/ap/ap_cas_bak.h ../../../../../../../inc/ap/ap_cas.h 2>null
rm -f null

echo "post build process completes"



