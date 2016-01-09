#!/bin/sh
AR=mipsel-linux-ar
ECHO=echo
CP=cp

$AR -d ../../../../../../../lib/lib_concerto/libcore_jasmine_drv_open.a cas_adapter.o
$AR -d ../../../../../../../lib/lib_concerto/libcore_jasmine_drv.a ads_adapter.o
$AR -d ../../../../../../../lib/lib_mips24k/libcore_ap_mdl.a ap_cas.o
mv ../../../../../../../src/kware/cas/cas_adapter.h ../../../../../../../src/kware/cas/cas_adapter_bak.h 2>null
mv ../../../../../../../src/kware/ads/ads_adapter.h ../../../../../../../src/kware/ads/ads_adapter_bak.h 2>null
mv ../../../../../../../inc/kware/cas_ware.h ../../../../../../../inc/kware/cas_ware_bak.h 2>null
mv ../../../../../../../inc/kware/ads_ware.h ../../../../../../../inc/kware/ads_ware_bak.h 2>null
mv ../../../../../../../inc/ap/ap_cas.h ../../../../../../../inc/ap/ap_cas_bak.h 2>null

$ECHO "pre build process completes"