@REM -->**************PLEASE CHANGE*******************
@SET TARGET_PLATFORM=mips1
@SET PRJ_NAME=jasmine_demo
@SET PRJ_CONFIG=release
@SET PRJ_CUSTOMER_NAME=demo_hd
@SET PRJ_CFG_NAME=jasmine_demo_hd

@REM -->**************PLEASE DO NOT CHANGE*******************

@REM <-- Create shell & Run background
@echo off
@if not "%1" == "h" mshta vbscript:createobject("wscript.shell").run("%~nx0 h",0)(window.close)&&exit
@REM -->

@REM STEP 1: Config target platform
@CALL toolchain.bat %TARGET_PLATFORM%

@REM STEP 2: Copy binary from binary to release\binary
@if exist %PRJ_CFG_NAME%\%PRJ_CONFIG% rd /S /Q %PRJ_CFG_NAME%\%PRJ_CONFIG%
@mkdir %PRJ_CFG_NAME%\%PRJ_CONFIG%
@cp -R ../../../../../../prj/dvbc/%PRJ_NAME%/binary %PRJ_CFG_NAME%/%PRJ_CONFIG%/
@cp -R ../../../../../../prj/dvbc/%PRJ_NAME%/customer/%PRJ_CUSTOMER_NAME%/%PRJ_CFG_NAME%/binary %PRJ_CFG_NAME%/%PRJ_CONFIG%/
@cp -R ../../../../../../uboot_avcpu/*.* %PRJ_CFG_NAME%/%PRJ_CONFIG%/binary/ota_binary/
@REM -->

@REM <-- Load project
@REM STEP 1: Import project
codeblocks.exe  %PRJ_CFG_NAME%\%PRJ_CFG_NAME%.cbp

@REM <--  Cleanup environment
@CALL toolchain.bat clean
@REM -->

