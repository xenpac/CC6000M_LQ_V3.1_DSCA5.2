@REM <-- Set all the variables**************PLEASE CHANGE*******************
@SET TARGET_PLATFORM=mips4k
@SET PRJ_NAME=barcode_client
@SET PRJ_CONFIG=release
@SET HW_PLATFORM=CHIP
@REM -->**************PLEASE CHANGE*******************

@REM <-- Create shell & Run background
@echo off
@if not "%1" == "h" mshta vbscript:createobject("wscript.shell").run("%~nx0 h",0)(window.close)&&exit
@REM -->

@REM <-- REM Initialize Environment
@REM STEP 1: Config target platform
@CALL toolchain.bat %TARGET_PLATFORM%
@REM -->

@REM STEP 2: Open project
@dos2unix %PRJ_NAME%/post_build.sh
@REM <-- Load project
codeblocks.exe  %PRJ_NAME%\%PRJ_NAME%.cbp
@REM -->