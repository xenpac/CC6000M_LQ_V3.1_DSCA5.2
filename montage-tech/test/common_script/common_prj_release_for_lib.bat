@REM ****************************************************************PLEASE DO NOT CHANGE*********************************
@setlocal EnableDelayedExpansion
@SET ROOT_PATH=%cd%
@SET OBJ_PLATFORM=%1
@SET PRJ_PATH=%2
@SET PRJ_NAME=%3
@SET LIB_RELEASE_PATH=%4
@SET PRJ_CONFIG=release
@SET WORKSPACE_PATH=%ROOT_PATH%\%PRJ_PATH%\%PRJ_NAME%
@SET PRJ_BINARY_PATH=%~5
@SET PRJ_RELEASE_TXT=%6
@SET MAGICBL_OTA_PATH=%ROOT_PATH%\config_prj\other

if @%LIB_RELEASE_PATH%==@ SET LIB_RELEASE_PATH=\\192.168.8.32\upload\release\prj_lib
echo %LIB_RELEASE_PATH%

@REM  ************************************************:COMPILE CORE**************************************************
@REM STEP 1: REM Initialize Environment
@CALL toolchain.bat %OBJ_PLATFORM%
@if %OBJ_PLATFORM%==jazz dos2unix %WORKSPACE_PATH_CORE%\linux_link_core_canna.sh
@if %OBJ_PLATFORM%==jazz dos2unix %WORKSPACE_PATH%\%PRJ_NAME%\post_build.sh

@REM STEP 2:********************************************COMPILE PROJECT**************************************************

@REM STEP 2.1: Clean environment
@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG% rd /Q /S %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%

@mkdir %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%
@REM STEP 2.2: Copy binary from binary to release\binary
@if "%PRJ_BINARY_PATH%"=="" SET PRJ_BINARY_PATH=%ROOT_PATH%\prj\dvbs\%PRJ_NAME%\binary
@cp -R %PRJ_BINARY_PATH% %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\
@cp -R uboot_avcpu\av_cpu.bin %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ota_binary\av_cpu.bin
@cp -R uboot_avcpu\av_cpu_64m.bin %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ota_binary\av_cpu_64m.bin
@cp -R uboot_avcpu\av_cpu_64m_sd.bin %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ota_binary\av_cpu_64m_sd.bin
@cp -R uboot_avcpu\btinit_spinor_ddr2.img %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ota_binary\boot_init_ddr2.img
@cp -R uboot_avcpu\btinit_spinor_ddr3.img %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ota_binary\boot_init_ddr3.img
@cp -R uboot_avcpu\uboot_concerto.img %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ota_binary\uboot.img
@cp -R uboot_avcpu\uboot_concerto_64m.img %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ota_binary\uboot_64m.img
@cp -R uboot_avcpu\uboot_concerto_64m_sd.img %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ota_binary\uboot_64m_sd.img

@if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\bootloader.bin del %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\bootloader.bin

@cp %MAGICBL_OTA_PATH%\magicbl\magicbl\binary\bootloader_magic_8M.bin %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ota_binary\bootloader_magic_8M.bin

@cp %MAGICBL_OTA_PATH%\ota\ota\binary\ota_gz.img %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ota_binary\ota_gz.img

@cp %WORKSPACE_PATH%\%PRJ_NAME%\post_build.sh %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\post_build.sh

@hg parent |gawk -F: '/changeset/ {printf "%%s", $2}' |sed -e 's/ //g' > tmpfile
@if %OBJ_PLATFORM%==jazz dd if=tmpfile bs=20 count=1 conv=notrunc seek=0 of=%WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ss_data_bigedian.bin
@if %OBJ_PLATFORM%==jazz dd if=tmpfile bs=20 count=1 conv=notrunc seek=0 of=%WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ss_data.bin
@if not %OBJ_PLATFORM%==jazz dd if=tmpfile bs=1 count=20 seek=0 of=%WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ss_data_bigedian.bin
@if not %OBJ_PLATFORM%==jazz dd if=tmpfile bs=1 count=20 seek=0 of=%WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\binary\ss_data.bin
@del tmpfile

@REM STEP 3.3: Import & compile project 
if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_NAME%.cbp codeblocks.exe  --no-splash-screen  --rebuild %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_NAME%.cbp --target=Release

@REM merge lib
mkdir lib_files
dir /s/b %WORKSPACE_PATH%\%PRJ_NAME%\obj\*.o >libs.txt
for /f "tokens=*" %%i in (libs.txt) do copy /y %%i lib_files
echo y|del /f %WORKSPACE_PATH%\%PRJ_NAME%\release\*.*
pushd lib_files
if exist ap_init.o del ap_init.o
if exist main.o del main.o
ar cru lib_%PRJ_NAME%.a *.o
copy /y lib_%PRJ_NAME%.a %WORKSPACE_PATH%\%PRJ_NAME%\release\
popd
copy /y %WORKSPACE_PATH%\%PRJ_NAME%\post_build.sh %WORKSPACE_PATH%\%PRJ_NAME%\release\
copy /y %WORKSPACE_PATH%\%PRJ_NAME%\link.lds %WORKSPACE_PATH%\%PRJ_NAME%\release\
rd /s /q lib_files
del libs.txt
for /f "tokens=4 delims=\" %%i in ('findstr "default" %ROOT_PATH%\.hg\hgrc') do set storage_name=%%i
@REM STEP 4: Copy release product
@REM STEP 4.1: Get changeset
@hg parent | gawk -F: '/changeset:/ {print $2}' |sed -e 's/ //g' > temp.txt
@for /f "tokens=*" %%i in (temp.txt) do mkdir %LIB_RELEASE_PATH%\%storage_name%\%storage_name%_%%i

@REM STEP 4.2: Copy release product
@for /f "tokens=*" %%i in (temp.txt) do set changeset=%%i
::xcopy /S /Y %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%\* %LIB_RELEASE_PATH%\%storage_name%\%storage_name%_%%i\
::echo d|xcopy /S /Y %ROOT_PATH%\prj %LIB_RELEASE_PATH%\%storage_name%\%storage_name%_%%i\prj\
::echo set prj_path=%PRJ_PATH%\%PRJ_NAME%\%PRJ_NAME%>%LIB_RELEASE_PATH%\%storage_name%\%storage_name%_%%i\prj.bat
::set changeset=%%i

for /f "tokens=*" %%i in (%WORKSPACE_PATH%\%PRJ_RELEASE_TXT%) do (
echo d|xcopy /y /i /s %%i %LIB_RELEASE_PATH%\%storage_name%\%storage_name%_%changeset%\%%i
)
copy /y %WORKSPACE_PATH%\%PRJ_NAME%\release\lib_%PRJ_NAME%.a %LIB_RELEASE_PATH%\%storage_name%\%storage_name%_%changeset%\lib\lib_concerto\
echo set prj_path=%PRJ_PATH%\%PRJ_NAME%\%PRJ_NAME%>%LIB_RELEASE_PATH%\%storage_name%\%storage_name%_%changeset%\prj.bat
echo set prj_name=%PRJ_NAME%>>%LIB_RELEASE_PATH%\%storage_name%\%storage_name%_%changeset%\prj.bat
@del temp.txt
if exist %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG% rd /s /q %WORKSPACE_PATH%\%PRJ_NAME%\%PRJ_CONFIG%
if exist %WORKSPACE_PATH%\%PRJ_NAME%\obj rd /s /q %WORKSPACE_PATH%\%PRJ_NAME%\obj
if exist %WORKSPACE_PATH%\%PRJ_NAME%\bin rd /s /q %WORKSPACE_PATH%\%PRJ_NAME%\bin
@CALL toolchain.bat clean
exit /b 0
:fail
@CALL toolchain.bat clean
exit /b -1



