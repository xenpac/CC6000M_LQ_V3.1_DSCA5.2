@REM <-- *********************************************************PLEASE DO NOT CHANGE****************************************

@SET OBJ_PLATFORM=%1
@SET PRJ_PATH=%2
@SET PRJ_NAME=%3
@SET PRJ_BINARY_PATH=%4
@SET PRJ_CONFIG=release
@SET ROOT_PATH=%cd%
@SET WORKSPACE_PATH=%ROOT_PATH%\%PRJ_PATH%
@SET MAGICBL_OTA_PATH=%ROOT_PATH%\config_prj\other

@REM -->STEP 0: Check if project.cbp file contains core files
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_NAME%.cbp type %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_NAME%.cbp > prj_cbp.txt
@call toolchain.bat win32
@sed -n '/filename=.*\.\.\\\\src\\\\/p' prj_cbp.txt > limit.txt
@sed -n '/filename=.*\.\.\\\\inc\\\\/p' prj_cbp.txt >> limit.txt
@sed '/\.\\\src\\\common\\\/d' limit.txt > limit1.txt

@findstr /R /i /v /c:"src.drv.nim.dvbc.m88dc2800.*" limit1.txt >limit2.txt
@findstr /R /i /v /c:"src.drv.nim.*" limit2.txt >limit3.txt
@type limit3.txt>limit.txt
@del limit1.txt
@del limit2.txt
@del limit3.txt
@call toolchain.bat clean
@del prj_cbp.txt
@findstr . limit.txt>nul || goto pass0
@goto bad0
:pass0
@if exist limit.txt del limit.txt

@REM STEP 1: REM Initialize Environment
@REM STEP 1.1: Config target platform
@CALL toolchain.bat %OBJ_PLATFORM%
@if %OBJ_PLATFORM%==jazz dos2unix %WORKSPACE_PATH%\%PRJ_NAME%\post_build.sh

@REM STEP 2: Load and build project
@REM STEP 2.1: Clean and make new environment
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG% rd /Q /S %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%
@mkdir %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\

@REM STEP 2.2: Copy binary from binary to release\binary
@if "%4"=="" SET PRJ_BINARY_PATH=%ROOT_PATH%\prj\dvbs\%PRJ_NAME%\binary
@if exist %PRJ_BINARY_PATH% cp -R %PRJ_BINARY_PATH% %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\bootloader.bin del %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\bootloader.bin

@cp %MAGICBL_OTA_PATH%\magicbl\magicbl\binary\bootloader_magic_8M.bin %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ota_binary\bootloader_magic_8M.bin
@cp %MAGICBL_OTA_PATH%\ota\ota\binary\ota_gz.img %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ota_binary\ota_gz.img
@cp %WORKSPACE_PATH%\%PRJ_NAME%\post_build.sh %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\post_build.sh
@hg tip |gawk -F: '/changeset/ {printf "%%s\0\0\0\0\0\0\0", $2}' |sed -e 's/ //g' > tmpfile
@dd if=tmpfile bs=1 count=20 seek=0 of=%WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ss_data.bin
@del tmpfile

@REM STEP 2.3: Import & compile project
if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_NAME%.cbp codeblocks.exe  --no-splash-screen  --rebuild %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_NAME%.cbp --target=Release >test\out.txt
@REM -->

@REM STEP 3: Clean environment
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\bin rd /Q /S %WORKSPACE_PATH%\%PRJ_NAME%\bin
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\.objs rd /Q /S %WORKSPACE_PATH%\%PRJ_NAME%\.objs
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\obj rd /Q /S %WORKSPACE_PATH%\%PRJ_NAME%\obj
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG% rd /Q /S %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\*.layout del %WORKSPACE_PATH%\%PRJ_NAME%\*.layout
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\*.depend del %WORKSPACE_PATH%\%PRJ_NAME%\*.depend
@CALL toolchain.bat clean

@REM <--  Return the compile result
@findstr /B /C:"0 errors" test\out.txt
@if not %errorlevel%==0 goto bad2
@findstr /C:": error:" test\out.txt
@if %errorlevel%==0 goto bad2
@if %OBJ_PLATFORM%==jazz goto skip
@findstr /C:"No such file or directory" test\out.txt
@if %errorlevel%==0 goto bad2
:skip
@if exist %ROOT_PATH%\test\prjtest_script_txt\%PRJ_NAME%_warning_exception.txt goto except_file
@findstr /C:": warning:" test\out.txt
@if %errorlevel%==0 goto bad1
@del test\out.txt
exit /b 0
:bad0
@echo ----You can not add the following core files of src or inc to compile in your project cbp file!!! > test\out.txt
@type limit.txt >> test\out.txt
@del limit.txt
:bad1
exit /b -1
@REM -->

:except_file
@findstr /R /i /v /g:test\prjtest_script_txt\%PRJ_NAME%_warning_exception.txt test\out.txt >test\outtemp.txt
@copy /Y test\outtemp.txt test\out.txt
@if exist test\outtemp.txt  del  test\outtemp.txt
@findstr /C:": warning:" test\out.txt
@if %errorlevel%==0 goto bad2
@del test\out.txt
exit /b 0
:bad2
exit /b -1
@REM -->