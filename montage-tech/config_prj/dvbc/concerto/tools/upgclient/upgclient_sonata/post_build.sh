LZMA=lzma
MV=mv

MKIMG=mkimg_mips
#rm -rf ./binary
mkdir -p release
mv upgclient_sonata.map ./release/
#mv ./${1} ./release/
#mv warriors.bin ./release/
mv upgclient_sonata.elf ./release/

cd ./release/


ANCHOR=../../../../../config_prj/other/anchor/bin/anchor.exe
ANCHOR2=../../../../../../config_prj/other/anchor/bin/anchor.exe
ANCHOR3=../../../../../../../config_prj/other/anchor/bin/anchor.exe

#echo --oformat elf32-tradlittlemips --gc-sections -nostdlib -T"../link.lds" -L../../../../../lib/lib_warriors -o ${1}  -Map upgclient_sonata.map > bb
#"find.exe" ./ -name "*.o" >> bb
#echo -leva_w -lcore_ginkgo -leva_w -lc -lm -lgcc -leva_w  >> bb
#cat bb | xargs mipsel-linux-ld.exe
#rm -rf bb
mipsel-linux-objcopy -O binary ${1} upgclient_sonata.bin
mipsel-linux-objdump -d upgclient_sonata.elf > upgclient_sonata.s

echo "post build process completes"



