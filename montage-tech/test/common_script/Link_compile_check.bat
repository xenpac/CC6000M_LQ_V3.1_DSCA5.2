@setlocal EnableDelayedExpansion
@SET ROOT_PATH=%cd%
@SET release_lib=\\192.168.8.32\upload\release\prj_lib
@SET drv_developmenent=%ROOT_PATH%\..\..\..\bundle\Drv_development\montage-tech\
@SET platform=%1
@REM get storage name
for /f "tokens=4 delims=\" %%i in ('findstr "default" %ROOT_PATH%\.hg\hgrc') do set storage_name=%%i

@REM Get changeset
CALL toolchain.bat jazz
@hg parent | gawk -F: '/changeset:/ {print $2}' |sed -e 's/ //g' > temp.txt
for /f "tokens=*" %%i in (temp.txt) do set changeset=%%i
set lib_path=%release_lib%\%storage_name%\%storage_name%_%changeset%

@copy release file to Drv_development repository
pushd %drv_developmenent%
hg pull
hg up -C
hg purge --all
xcopy /y /s %lib_path%\. %drv_developmenent%
if not errorlevel 0 goto fail
mkdir uboot_avcpu
call prj.bat
echo f|xcopy /y %prj_path%\release\binary\ota_binary\av_cpu.bin uboot_avcpu\av_cpu.bin
echo f|xcopy /y %prj_path%\release\binary\ota_binary\av_cpu_64m.bin uboot_avcpu\av_cpu_64m.bin
echo f|xcopy /y %prj_path%\release\binary\ota_binary\av_cpu_64m_sd.bin uboot_avcpu\av_cpu_64m_sd.bin
echo f|xcopy /y %prj_path%\release\binary\ota_binary\boot_init_ddr2.img uboot_avcpu\btinit_spinor_ddr2.img
echo f|xcopy /y %prj_path%\release\binary\ota_binary\boot_init_ddr3.img uboot_avcpu\btinit_spinor_ddr3.img
echo f|xcopy /y %prj_path%\release\binary\ota_binary\uboot.img uboot_avcpu\uboot_concerto.img
echo f|xcopy /y %prj_path%\release\binary\ota_binary\uboot_64m.img uboot_avcpu\uboot_concerto_64m.img
echo f|xcopy /y %prj_path%\release\binary\ota_binary\uboot_64m_sd.img uboot_avcpu\uboot_concerto_64m_sd.img
del /s src\*.c

::change prj
findstr /C:"\main.c" %prj_path%\%Prj_name%.cbp >%prj_path%\..\%Prj_name%.txt
echo ^<Option compilerVar="CC" /^>>>%prj_path%\..\%Prj_name%.txt
echo ^</Unit^>>>%prj_path%\..\%Prj_name%.txt
findstr /C:"\ap_init.c" %prj_path%\%Prj_name%.cbp >>%prj_path%\..\%Prj_name%.txt
echo ^<Option compilerVar="CC" /^>>>%prj_path%\..\%Prj_name%.txt
echo ^</Unit^>>>%prj_path%\..\%Prj_name%.txt
echo ^<Linker^>>>%prj_path%\..\%Prj_name%.txt
echo ^<Add library="_%Prj_name%" /^>>>%prj_path%\..\%Prj_name%.txt
echo ^</Linker^>>>%prj_path%\..\%Prj_name%.txt
echo ^</Project^>>>%prj_path%\..\%Prj_name%.txt
echo ^</CodeBlocks_project_file^>>>%prj_path%\..\%Prj_name%.txt
sed -i '/\.c">/d' %prj_path%\%Prj_name%.cbp
sed -i '/compilerVar="CC"/d' %prj_path%\%Prj_name%.cbp
sed -i '/\/Unit/d' %prj_path%\%Prj_name%.cbp
sed -i '/\/Project/d' %prj_path%\%Prj_name%.cbp
sed -i '/\/CodeBlocks/d' %prj_path%\%Prj_name%.cbp
type %prj_path%\..\%Prj_name%.txt >>%prj_path%\%Prj_name%.cbp
CALL toolchain.bat clean
CALL toolchain.bat %platform%
codeblocks.exe  --no-splash-screen  --rebuild %prj_path%\%Prj_name%.cbp --target=Release >%ROOT_PATH%\test\%Prj_name%_release_error_log.txt
CALL toolchain.bat clean

@findstr /B /C:"0 errors" %ROOT_PATH%\test\%Prj_name%_release_error_log.txt
@if not %errorlevel%==0 goto fail
@findstr /C:": error:" %ROOT_PATH%\test\%Prj_name%_release_error_log.txt
@if %errorlevel%==0 goto fail
del %ROOT_PATH%\test\%Prj_name%_release_error_log.txt
exit /b 0

:fail
exit /b -1