#backup
#mv ../../../../lib/lib_sonata/libcore_iris_drv.a ../../../../lib/lib_sonata/libcore_iris_drv.a.org

cp ../../../../lib/lib_sonata/libcore_iris_drv.a ./

ar -x libcore_iris_drv.a

rm -f init_sonata.o
rm -f libcore_iris_drv.a

cp ./init/init_sonata.o ./

ar -q libcore_iris_drv.a *.o
rm -f *.o

cp libcore_iris_drv.a ../../../../lib/lib_sonata/

echo "pre build process completes"



