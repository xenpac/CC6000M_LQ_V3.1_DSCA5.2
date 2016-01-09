@REM -->**************PLEASE CHANGE*******************
@SET TARGET_PLATFORM=mips1
@SET PRJ_NAME=jasmine_demo_hd_ota
@SET PRJ_CONFIG=release

@REM -->**************PLEASE DO NOT CHANGE*******************

@REM <-- Create shell & Run background
@echo off
@if not "%1" == "h" mshta vbscript:createobject("wscript.shell").run("%~nx0 h",0)(window.close)&&exit
@REM -->

@REM STEP 1: Config target platform
@CALL toolchain.bat %TARGET_PLATFORM%

@REM STEP 2: Copy binary from binary to release\binary
@if exist %PRJ_NAME%\%PRJ_CONFIG% rd /S /Q %PRJ_NAME%\%PRJ_CONFIG%
@mkdir %PRJ_NAME%\%PRJ_CONFIG%
@REM -->

@REM <-- Load project
@REM STEP 1: Import project
codeblocks.exe  %PRJ_NAME%\%PRJ_NAME%.cbp

@REM <--  Cleanup environment
@CALL toolchain.bat clean
@REM -->