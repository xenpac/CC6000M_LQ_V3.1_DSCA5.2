@REM ****************************************************************PLEASE DO NOT CHANGE*********************************

@setlocal EnableDelayedExpansion
@SET ROOT_PATH=%cd%
@SET OBJ_PLATFORM=%1
@SET CORE_PATH=%2
@SET CORE_CBP_NAME=%3
@SET WORKSPACE_PATH_CORE=%ROOT_PATH%\%CORE_PATH%

@SET PRJ_PATH=%4
@SET PRJ_NAME=%5
@SET PRJ_CONFIG=release
@SET WORKSPACE_PATH=%ROOT_PATH%\%PRJ_PATH%

@SET CHIP_NAME=%6
@SET CHIP=%7
@SET FP=%8
@SET RFILE=%CHIP%_%PRJ_NAME%

@SET RELEASE_PATH=%9
if "%9"=="" SET RELEASE_PATH=\\192.168.8.32\upload\release

@SET RELEASE_PATH_PERF_TEST=%RELEASE_PATH%\perf_test
@SET MAGICBL_OTA_PATH=%ROOT_PATH%\config_prj\other
@SET LIB_PATH=%ROOT_PATH%\lib\lib_%CHIP_NAME%\perf_lib


@REM  ************************************************:COMPILE CORE**************************************************

@REM STEP 1: REM Initialize Environment
@CALL toolchain.bat %OBJ_PLATFORM%
@if %OBJ_PLATFORM%==jazz dos2unix %WORKSPACE_PATH_CORE%\linux_link_core_canna.sh
@if %OBJ_PLATFORM%==jazz dos2unix %WORKSPACE_PATH%\%PRJ_NAME%\post_build.sh

@REM STEP 2: Load and build project
if exist %WORKSPACE_PATH_CORE%\%CORE_CBP_NAME% codeblocks.exe --no-splash-screen --rebuild %WORKSPACE_PATH_CORE%\%CORE_CBP_NAME% --target=Release
@REM -->


@REM STEP 3 ************************************************:COMPILE PROJECT**************************************************
@CALL toolchain.bat jazz
@REM STEP 3.0: Read .a files
@dir /b %LIB_PATH%\*.a > a_files_org.txt
@sed -e 's/lib//g' a_files_org.txt > a_files_1.txt
@sed -e 's/.a$//g' a_files_1.txt > a_files.txt
@if %PERF_MODE%==1 sed -i '/log_dummy/d' a_files.txt
@del a_files_*.txt
@type %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_NAME%.cbp > 1.txt

@CALL toolchain.bat %OBJ_PLATFORM%
@for /f  %%k in (a_files.txt) do (
@REM STEP 5.1: 
@sed -e 's/log_dummy/%%k/' 1.txt > %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_NAME%.cbp

@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG% rd /Q /S %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%

@REM STEP 5.2: Copy binary from binary to release\binary
@mkdir %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%
@if exist %ROOT_PATH%\prj\dvbs\%PRJ_NAME%\binary cp -R %ROOT_PATH%\prj\dvbs\%PRJ_NAME%\binary %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\

@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\bootloader.bin del %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\bootloader.bin

@cp %MAGICBL_OTA_PATH%\magicbl\magicbl\binary\bootloader_magic_8M.bin %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ota_binary\bootloader_magic_8M.bin

@cp %MAGICBL_OTA_PATH%\ota\ota\binary\ota_gz.img %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ota_binary\ota_gz.img

@if exist %WORKSPACE_PATH_OTA%\%OTA_CONFIG%\binary\ cp %WORKSPACE_PATH_OTA%\%OTA_CONFIG%\binary\ %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary/ota_binary\

@cp %WORKSPACE_PATH%\%PRJ_NAME%\post_build.sh %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\post_build.sh

@hg parent |gawk -F: '/changeset/ {printf "%%s\0\0\0\0\0\0\0", $2}' |sed -e 's/ //g' > tmpfile
@if %OBJ_PLATFORM%==jazz dd if=tmpfile bs=20 count=1 conv=notrunc seek=0 of=%WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ss_data_bigedian.bin
@if %OBJ_PLATFORM%==jazz dd if=tmpfile bs=20 count=1 conv=notrunc seek=0 of=%WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ss_data.bin
@if not %OBJ_PLATFORM%==jazz dd if=tmpfile bs=1 count=20 seek=0 of=%WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ss_data_bigedian.bin
@if not %OBJ_PLATFORM%==jazz dd if=tmpfile bs=1 count=20 seek=0 of=%WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ss_data.bin
@del tmpfile

@REM STEP 5.3: Import & compile project 
if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_NAME%.cbp codeblocks.exe  --no-splash-screen  --rebuild %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_NAME%.cbp --target=Release
@REM -->

@REM STEP 6: Copy release product
@REM STEP 6.1: Get changeset
@hg parent | gawk -F: '/changeset:/ {print $2}' |sed -e 's/ //g' > temp.txt

@REM STEP 6.2: New the folder to store products
@for /f "tokens=*" %%i in (temp.txt) do (
@if "%%k"=="log_dummy" (
mkdir %RELEASE_PATH%\%PRJ_NAME%\%PRJ_NAME%_%%i
mkdir %RELEASE_PATH%\%PRJ_NAME%\%PRJ_NAME%_%%i\%PRJ_NAME%_%%i_binary
) else (
mkdir %RELEASE_PATH_PERF_TEST%\%PRJ_NAME%\%PRJ_NAME%_%%i
mkdir %RELEASE_PATH_PERF_TEST%\%PRJ_NAME%\%PRJ_NAME%_%%i\%%k\binary
)
)

@REM STEP 6.3: Creat zip file
@hg parent | gawk -F: '/changeset:/ {print $3}' |sed -e 's/ //g' > temp1.txt

@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\boot_init.img cp %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\boot_init.img boot_init.img
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\boot.img cp %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\boot.img boot.img
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\%PRJ_NAME%.s cp %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\%PRJ_NAME%.s %PRJ_NAME%.s
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\%PRJ_NAME%.elf cp %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\%PRJ_NAME%.elf %PRJ_NAME%.elf
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\image.map cp %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\image.map image.map
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\jazz_download.img cp %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\jazz_download.img jazz_download.img

@for /f "tokens=*" %%i in (temp.txt) do (
cp %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\flash.bin %RFILE%_%FP%_%%i.bin
for /f "tokens=*" %%j in (temp1.txt) do (
cp %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\script.cfg %RFILE%_%%j.cfg
zip -r %RFILE%_%FP%_%%i.zip %RFILE%_%FP%_%%i.bin %RFILE%_%%j.cfg boot_init.img boot.img %PRJ_NAME%.s image.map %PRJ_NAME%.elf jazz_download.img
del %RFILE%_%FP%_%%i.bin
del %RFILE%_%%j.cfg
)
)
@if exist boot_init.img del boot_init.img
@if exist boot.img del boot.img
@if exist image.map del image.map
@if exist %PRJ_NAME%.s del %PRJ_NAME%.s
@if exist %PRJ_NAME%.elf del %PRJ_NAME%.elf
@if exist jazz_download.img del jazz_download.img

@REM STEP 6.4: Copy release product
@for /f "tokens=*" %%i in (temp.txt) do (
@if "%%k"=="log_dummy" (
move %RFILE%_%FP%_%%i.zip %RELEASE_PATH%\%PRJ_NAME%\%PRJ_NAME%_%%i\
copy /y %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\*.* %RELEASE_PATH%\%PRJ_NAME%\%PRJ_NAME%_%%i\%PRJ_NAME%_%%i_binary\
copy /y %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\*.s %RELEASE_PATH%\%PRJ_NAME%\%PRJ_NAME%_%%i\%PRJ_NAME%_%%i_binary\
copy /y %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\wizard*.*  %RELEASE_PATH%\%PRJ_NAME%\%PRJ_NAME%_%%i\%PRJ_NAME%_%%i_binary\
) else (
del %RFILE%_%FP%_%%i.zip
copy /y %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\flash.bin %RELEASE_PATH_PERF_TEST%\%PRJ_NAME%\%PRJ_NAME%_%%i\%%k\binary\
)
)
@del temp.txt
@del temp1.txt
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG% rd /Q /S %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%
)
@type 1.txt > %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_NAME%.cbp

@REM STEP 7:  Cleanup environment
@del 1.txt
@del a_files.txt
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\bin rd /Q /S %WORKSPACE_PATH%\%PRJ_NAME%\bin
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\.objs rd /Q /S %WORKSPACE_PATH%\%PRJ_NAME%\.objs
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\obj rd /Q /S %WORKSPACE_PATH%\%PRJ_NAME%\obj
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\*.layout del %WORKSPACE_PATH%\%PRJ_NAME%\*.layout
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\*.depend del %WORKSPACE_PATH%\%PRJ_NAME%\*.depend

@if exist %WORKSPACE_PATH_CORE%\*.a del %WORKSPACE_PATH_CORE%\*.a
@del /s /q %WORKSPACE_PATH_CORE%\*.layout
@del /s /q %WORKSPACE_PATH_CORE%\*.depend
@echo off
if exist %WORKSPACE_PATH_CORE% (
pushd %WORKSPACE_PATH_CORE%
set fn=bin
set m0=0
:loop0
set /a m0+=1
if not "!fn:~%m0%,1!" equ "" goto loop0
for /f %%k in ('dir /s/b/ad') do (
set aa=%%k
if "!aa:~-%m0%!" equ "%fn%" rd /s/q %%k 2>nul)
set fn=obj
set m1=0
:loop1
set /a m1+=1
if not "!fn:~%m1%,1!" equ "" goto loop1 
for /f %%k in ('dir /s/b/ad') do (
set aa=%%k
if "!aa:~-%m1%!" equ "%fn%" rd /s/q %%k 2>nul)
popd
)
@CALL toolchain.bat clean
@REM -->
