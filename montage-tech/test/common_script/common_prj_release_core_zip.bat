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

@SET WORKSPACE_PATH=%ROOT_PATH%\%PRJ_PATH%\%PRJ_NAME%
@SET CHIP=%6
@SET FP=%7
@SET RFILE=%CHIP%_%PRJ_NAME%

@SET RELEASE_PATH=%8

@SET PRJ_BINARY_PATH=%~9

if "%8"=="" SET RELEASE_PATH=\\192.168.8.32\upload\release

@SET MAGICBL_OTA_PATH=%ROOT_PATH%\config_prj\other

@SET OUTPUT=%PRJ_NAME%_release_error_log.txt

@REM  ************************************************:COMPILE CORE**************************************************
@REM STEP 1: REM Initialize Environment
@CALL toolchain.bat %OBJ_PLATFORM%
@if %OBJ_PLATFORM%==jazz dos2unix %WORKSPACE_PATH_CORE%\linux_link_core_canna.sh
@if %OBJ_PLATFORM%==jazz dos2unix %WORKSPACE_PATH%\%PRJ_NAME%\post_build.sh

@REM STEP 2: Import & compile project
if exist %WORKSPACE_PATH_CORE%\%CORE_CBP_NAME% codeblocks.exe --no-splash-screen --rebuild %WORKSPACE_PATH_CORE%\%CORE_CBP_NAME% --target=Release > test\%OUTPUT%
@REM -->


@REM STEP 4:********************************************COMPILE PROJECT**************************************************

@REM STEP 4.1: Clean environment
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG% rd /Q /S %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%

@REM STEP 4.2: Copy binary from binary to release\binary
@mkdir -p %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ota_binary
@REM STEP 2.2: Copy binary from binary to release\binary
@if "%PRJ_BINARY_PATH%"=="" SET PRJ_BINARY_PATH=%ROOT_PATH%\prj\dvbs\%PRJ_NAME%\binary

@cp -R uboot_avcpu/*.* %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ota_binary\
@cp -R %PRJ_BINARY_PATH% %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\


@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\bootloader.bin del %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\bootloader.bin

@cp %MAGICBL_OTA_PATH%\magicbl\magicbl\binary\bootloader_magic_8M.bin %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ota_binary\bootloader_magic_8M.bin

@cp %MAGICBL_OTA_PATH%\ota\ota\binary\ota_gz.img %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ota_binary\ota_gz.img

@cp %WORKSPACE_PATH%\%PRJ_NAME%\post_build.sh %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\post_build.sh

@hg parent |gawk -F: '/changeset/ {printf "%%s", $2}' |sed -e 's/ //g' > tmpfile
@shift /8
@CALL toolchain.bat clean
@CALL toolchain.bat jazz
@printf "%%s" %9 >>tmpfile
@CALL toolchain.bat clean
@CALL toolchain.bat %OBJ_PLATFORM%
@if %OBJ_PLATFORM%==jazz dd if=tmpfile bs=20 count=1 conv=notrunc seek=0 of=%WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ss_data_bigedian.bin
@if %OBJ_PLATFORM%==jazz dd if=tmpfile bs=20 count=1 conv=notrunc seek=0 of=%WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ss_data.bin
@if not %OBJ_PLATFORM%==jazz dd if=tmpfile bs=1 count=20 seek=0 of=%WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ss_data_bigedian.bin
@if not %OBJ_PLATFORM%==jazz dd if=tmpfile bs=1 count=20 seek=0 of=%WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ss_data.bin
@del tmpfile

@REM STEP 4.3: Import & compile project 
if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_NAME%.cbp codeblocks.exe  --no-splash-screen  --rebuild %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_NAME%.cbp --target=Release >> test\%OUTPUT%
@REM -->
call test\common_script\common_merge_lib.bat %WORKSPACE_PATH% %PRJ_NAME% %OBJ_PLATFORM%
@REM STEP 5: Copy release product
@REM STEP 5.1: Get changeset
@hg parent | gawk -F: '/changeset:/ {print $2}' |sed -e 's/ //g' > temp.txt

@REM STEP 5.2: New the folder to store products
@for /f "tokens=*" %%i in (temp.txt) do (
@mkdir %RELEASE_PATH%\%PRJ_NAME%\%PRJ_NAME%_%%i
@mkdir %RELEASE_PATH%\%PRJ_NAME%\%PRJ_NAME%_%%i\%PRJ_NAME%_%%i_binary
)

@REM STEP 5.3: Creat zip file
@hg parent | gawk -F: '/changeset:/ {print $3}' |sed -e 's/ //g' > temp1.txt

@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\boot_init.img cp %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\boot_init.img boot_init.img
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\boot.img cp %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\boot.img boot.img
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\%PRJ_NAME%.s cp %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\%PRJ_NAME%.s %PRJ_NAME%.s
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\%PRJ_NAME%.elf cp %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\%PRJ_NAME%.elf %PRJ_NAME%.elf
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\image.map cp %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\image.map image.map
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\jazz_download.img cp %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\jazz_download.img jazz_download.img

@for /f "tokens=*" %%i in (temp.txt) do (
cp %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\flash.bin %RFILE%_%FP%_%%i.bin
@for /f "tokens=*" %%j in (temp1.txt) do (
cp %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\script.cfg %RFILE%_%%j.cfg
@zip -r %RFILE%_%FP%_%%i.zip %RFILE%_%FP%_%%i.bin %RFILE%_%%j.cfg boot_init.img boot.img %PRJ_NAME%.s image.map %PRJ_NAME%.elf jazz_download.img
@del %RFILE%_%FP%_%%i.bin
@del %RFILE%_%%j.cfg
)
)
@if exist boot_init.img del boot_init.img
@if exist boot.img del boot.img
@if exist image.map del image.map
@if exist %PRJ_NAME%.s del %PRJ_NAME%.s
@if exist %PRJ_NAME%.elf del %PRJ_NAME%.elf
@if exist jazz_download.img del jazz_download.img

@REM STEP 5.4: Copy release product
@for /f "tokens=*" %%i in (temp.txt) do (
move %RFILE%_%FP%_%%i.zip %RELEASE_PATH%\%PRJ_NAME%\%PRJ_NAME%_%%i\
xcopy /s/y %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\* %RELEASE_PATH%\%PRJ_NAME%\%PRJ_NAME%_%%i\%PRJ_NAME%_%%i_binary\
copy %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\*.s %RELEASE_PATH%\%PRJ_NAME%\%PRJ_NAME%_%%i\%PRJ_NAME%_%%i_binary\
copy %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\wizard*.*  %RELEASE_PATH%\%PRJ_NAME%\%PRJ_NAME%_%%i\%PRJ_NAME%_%%i_binary\
)
@del temp.txt
@del temp1.txt


@REM STEP 6:  Cleanup environment
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\bin rd /Q /S %WORKSPACE_PATH%\%PRJ_NAME%\bin
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\.objs rd /Q /S %WORKSPACE_PATH%\%PRJ_NAME%\.objs
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\obj rd /Q /S %WORKSPACE_PATH%\%PRJ_NAME%\obj
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG% rd /Q /S %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\*.layout del %WORKSPACE_PATH%\%PRJ_NAME%\*.layout
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\*.depend del %WORKSPACE_PATH%\%PRJ_NAME%\*.depend
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\*.elf del %WORKSPACE_PATH%\%PRJ_NAME%\*.elf

@if exist %WORKSPACE_PATH_CORE%\*.a del %WORKSPACE_PATH_CORE%\*.a
@del /s /q %WORKSPACE_PATH_CORE%\*.layout
@del /s /q %WORKSPACE_PATH_CORE%\*.depend
@if exist %WORKSPACE_PATH_CORE% (
@pushd %WORKSPACE_PATH_CORE%
@set fn=bin
@set m0=0
:loop0
@set /a m0+=1
@if not "!fn:~%m0%,1!" equ "" goto loop0
@for /f %%k in ('dir /s/b/ad') do (
@set aa=%%k
@if "!aa:~-%m0%!" equ "%fn%" rd /s/q %%k 2>nul)
@set fn=obj
@set m1=0
:loop1
@set /a m1+=1
@if not "!fn:~%m1%,1!" equ "" goto loop1 
@for /f %%k in ('dir /s/b/ad') do (
@set aa=%%k
@if "!aa:~-%m1%!" equ "%fn%" rd /s/q %%k 2>nul)
@popd
)
@REM <--  Return the compile result
@if not exist test\%OUTPUT% echo %OUTPUT% not exist!!!>test\%OUTPUT% goto bad
@findstr . test\%OUTPUT% >nul&&echo "no empty"||echo "empty %OUTPUT% because this script not execute; please ask SQA for help!">test\%OUTPUT% &&goto bad
@if exist test\prjtest_script_txt\core_ginkgo_warning_exception.txt goto ext
@goto normal

:ext
@findstr /R /i /v /g:test\prjtest_script_txt\core_ginkgo_warning_exception.txt test\%OUTPUT% >test\outtemp.txt
@copy /Y test\outtemp.txt test\%OUTPUT%
@if exist test\outtemp.txt  del  test\outtemp.txt

:normal
@findstr /B /C:"0 errors" test\%OUTPUT%
@if not %errorlevel%==0 goto bad
@findstr /C:": error:" test\%OUTPUT%
@if %errorlevel%==0 goto bad
@findstr /C:"make: *** No rule to make target" test\%OUTPUT%
@if %errorlevel%==0 goto bad
@findstr /C:"Process terminated with status 1" test\%OUTPUT%
@if %errorlevel%==0 goto bad
@findstr /C:": warning:" test\%OUTPUT%
@if %errorlevel%==0 goto bad
@findstr /C:"No such file or directory" test\%OUTPUT%
@if %errorlevel%==0 goto bad
@del test\%OUTPUT%
@CALL toolchain.bat clean
exit /b 0

:bad
echo For warnings,you just need to repair which refered in the txt not all >>test\%OUTPUT%
@CALL toolchain.bat clean
exit /b -1
@REM -->
