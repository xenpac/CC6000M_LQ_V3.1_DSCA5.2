/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
// hwcfg_sonata_default_unittest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "gtest\gtest.h"
#include "..\..\uioconfig\system_configure_native_interface.h"


NativePropertyArray_t *temp = new NativePropertyArray_t;

TEST(hwcfg_concerto_default, ProjectInit) {



}

TEST(hwcfg_concerto_default, Load) {

    EXPECT_EQ(0, Load(L"D:\\work\\izard\\montage-tech\\hwcfg_concerto_default\\hwcfg_concerto_default\\flash.bin",temp));


}

TEST(hwcfg_concerto_default, SaveToFlash) {

    EXPECT_EQ(0, SaveToFlash(L"d:\\tmpFlash.bin",temp));

}

TEST(hwcfg_concerto_default, Clean) {

    EXPECT_EQ(0, Clean());
    delete temp;
}

int _tmain(int argc, _TCHAR* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
}
