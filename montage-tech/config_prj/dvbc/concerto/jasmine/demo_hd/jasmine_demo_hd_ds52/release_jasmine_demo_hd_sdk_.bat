@REM <-- Set some variables**********************PLEASE CHANGE**********************************
@SET OBJ_PLATFORM=mips1
@SET PRJ_NAME1=jasmine_demo_hd
@SET PRJ_NAME2=jasmine_demo_hd_ota
@SET PRJ_PATH=config_prj\dvbc\concerto\jasmine\demo_hd
@SET PRJ_COPY_TXT=jasmine_demo_hd_sdk_release_src.txt
@SET PRJ_BIN_PATH=prj\dvbc\jasmine_demo\binary
@SET PRJ1_customer_BIN_PATH=prj\dvbc\jasmine_demo\customer\demo_hd\%PRJ_NAME1%\binary
@SET OUTPUT=%PRJ_NAME%_release_error_log.txt
@SET RELEASE_PATH=\\192.168.8.32\upload\release
@SET Storage_name=Jasmine
@SET PRJ_CONFIG=release
@SET ROOT_PATH=%cd%
::@SET PRINT_PRJ=off

@REM <--CALL THE BAT TO OFF PRINT
@if @%PRINT_PRJ%==@off call test\common_script\common_core_print_off.bat %PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%
		
@REM<-- Set the toolchain******************************************************************************
call toolchain.bat %OBJ_PLATFORM%

@REM <-- Copy binary***********************************************************************************
if exist %PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%\%PRJ_CONFIG%\binary\ota_binary rd /s /q %PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%\%PRJ_CONFIG%\binary\ota_binary
mkdir %PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%\%PRJ_CONFIG%\binary\ota_binary
mkdir %PRJ_PATH%\%PRJ_NAME2%\%PRJ_NAME2%\%PRJ_CONFIG%
@cp -R uboot_avcpu/*.* %PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%\%PRJ_CONFIG%\binary\ota_binary
@cp -R %PRJ_BIN_PATH% %PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%\%PRJ_CONFIG%\
@cp -R %PRJ1_customer_BIN_PATH% %PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%\%PRJ_CONFIG%\

@REM<-- Get changeset********************************************
hg parent | gawk -F: '/changeset:/ {print $2}' |sed -e 's/ //g'>temp.txt

@REM <-- Compile PRJ********************************************
if exist %PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%\%PRJ_NAME1%.cbp codeblocks.exe  --no-splash-screen  --rebuild %PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%\%PRJ_NAME1%.cbp --target=Release > test\%OUTPUT%
@printf "%%s" %1 >>tmpfile
@if %OBJ_PLATFORM%==jazz dd if=tmpfile bs=20 count=1 conv=notrunc seek=0 of=%PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%\%PRJ_CONFIG%\binary\ss_data_bigedian.bin
@if %OBJ_PLATFORM%==jazz dd if=tmpfile bs=20 count=1 conv=notrunc seek=0 of=%PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%\%PRJ_CONFIG%\binary\ss_data.bin
@if not %OBJ_PLATFORM%==jazz dd if=tmpfile bs=1 count=20 seek=0 of=%PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%\%PRJ_CONFIG%\binary\ss_data_bigedian.bin
@if not %OBJ_PLATFORM%==jazz dd if=tmpfile bs=1 count=20 seek=0 of=%PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%\%PRJ_CONFIG%\binary\ss_data.bin

for /f "tokens=*" %%i in (temp.txt) do (
mkdir %RELEASE_PATH%\%Storage_name%\%Storage_name%_%%i
xcopy /s/y %PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%\%PRJ_CONFIG%\binary\* %RELEASE_PATH%\%Storage_name%\%Storage_name%_%%i\binary\%PRJ_NAME1%_%%i\
)
rd /s /q %PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%\bin
rd /s /q %PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%\obj
rd /s /q %PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%\release
if exist %PRJ_PATH%\%PRJ_NAME2%\%PRJ_NAME2%\%PRJ_NAME2%.cbp codeblocks.exe  --no-splash-screen  --rebuild %PRJ_PATH%\%PRJ_NAME2%\%PRJ_NAME2%\%PRJ_NAME2%.cbp --target=Release >> test\%OUTPUT%
for /f "tokens=*" %%i in (temp.txt) do (
xcopy /s/y %PRJ_PATH%\%PRJ_NAME2%\%PRJ_NAME2%\%PRJ_CONFIG%\* %RELEASE_PATH%\%Storage_name%\%Storage_name%_%%i\binary\%PRJ_NAME2%_%%i\
)
rd /s /q %PRJ_PATH%\%PRJ_NAME2%\%PRJ_NAME2%\bin
rd /s /q %PRJ_PATH%\%PRJ_NAME2%\%PRJ_NAME2%\obj
rd /s /q %PRJ_PATH%\%PRJ_NAME2%\%PRJ_NAME2%\release

@REM <--Copy release product---you can copy files you any want*******************************************
for /f "tokens=*" %%i in (temp.txt) do (
for /f "tokens=*" %%x in (%PRJ_PATH%\%PRJ_NAME1%\%PRJ_COPY_TXT%) do (
echo f|xcopy /y /i /s %%x %PRJ_NAME1%_%%i\%PRJ_NAME1%\%%x
)
del /Q %PRJ_NAME1%_%%i\%PRJ_NAME1%\%PRJ_PATH%\%PRJ_NAME1%\%PRJ_COPY_TXT%
del /Q %PRJ_NAME1%_%%i\%PRJ_NAME1%\%PRJ_PATH%\%PRJ_NAME1%\release_%PRJ_NAME1%.bat
del /Q %PRJ_NAME1%_%%i\%PRJ_NAME1%\%PRJ_PATH%\%PRJ_NAME1%\release_%PRJ_NAME1%_sdk.bat
)

@REM<-- Set the toolchain********************************************************************************
call toolchain.bat clean
call toolchain.bat jazz

@REM <--Copy release product---copy project1 src files****************************************************
findstr /C:"Unit filename" %PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%\%PRJ_NAME1%.cbp >test\src_temp.txt
for /f tokens^=2^ delims^=^"  %%i in (test\src_temp.txt) do echo %%i>>test\split_src.txt
sed -i 's/..\\\..\\\..\\\..\\\..\\\..\\\..\\\//g' test\split_src.txt
sed -i 's/..\\/..\\/..\\/..\\/..\\/..\\/..\\///g' test\split_src.txt
sed -i 's/\//\\\/g' test\split_src.txt
for /f "tokens=*" %%i in (temp.txt) do (
for /f "tokens=*" %%x in (test\split_src.txt) do (
echo f|xcopy /y /i /s %%x %PRJ_NAME1%_%%i\%PRJ_NAME1%\%%x
)
xcopy /y /i prj\dvbc\jasmine_demo\kware\ads\ads_adapter.c %PRJ_NAME1%_%%i\%PRJ_NAME1%\prj\dvbc\jasmine_demo\kware\ads\ads_adapter.c
)

@REM <--Copy release product1---copy project inc&lib files**************************************************
findstr /C:"Add directory" %PRJ_PATH%\%PRJ_NAME1%\%PRJ_NAME1%\%PRJ_NAME1%.cbp>>test\inc_temp.txt
for /f "tokens=3 delims=}&"  %%i in (test\inc_temp.txt) do echo %%i>>test\split_inc.txt
sed -i 's/\//\\\/g' test\split_inc.txt
sed -i 's/\\\prj\\\/prj\\\/g' test\split_inc.txt
sed -i 's/\\\src\\\/src\\\/g' test\split_inc.txt
sed -i 's/\\\inc\\\/inc\\\/g' test\split_inc.txt
sed -i 's/\\\lib\\\/lib\\\/g' test\split_inc.txt
for /f "tokens=*" %%i in (temp.txt) do (
for /f "tokens=*" %%j in (test\split_inc.txt) do (
echo f|xcopy /y /i %%j\*.h %PRJ_NAME1%_%%i\%PRJ_NAME1%\%%j\*.h
echo f|xcopy /y /i %%j\*.a %PRJ_NAME1%_%%i\%PRJ_NAME1%\%%j\*.a
)
)

@REM <--Copy release product2---copy project1 src files****************************************************
findstr /C:"Unit filename" %PRJ_PATH%\%PRJ_NAME2%\%PRJ_NAME2%\%PRJ_NAME2%.cbp >test\src_temp2.txt
for /f tokens^=2^ delims^=^"  %%i in (test\src_temp2.txt) do echo %%i>>test\split_src2.txt
sed -i 's/..\\\..\\\..\\\..\\\..\\\..\\\..\\\//g' test\split_src2.txt
sed -i 's/..\\/..\\/..\\/..\\/..\\/..\\/..\\///g' test\split_src2.txt
sed -i 's/\//\\\/g' test\split_src2.txt
for /f "tokens=*" %%i in (temp.txt) do (
for /f "tokens=*" %%x in (test\split_src2.txt) do (
echo f|xcopy /y /i %%x %PRJ_NAME1%_%%i\%PRJ_NAME1%\%%x
)
xcopy /y /i prj\dvbc\jasmine_demo\kware\ads\ads_adapter.c %PRJ_NAME1%_%%i\%PRJ_NAME1%\prj\dvbc\jasmine_demo\kware\ads\ads_adapter.c
)

@REM <--Copy release product2---copy project inc&lib files**************************************************
findstr /C:"Add directory" %PRJ_PATH%\%PRJ_NAME2%\%PRJ_NAME2%\%PRJ_NAME2%.cbp>>test\inc_temp2.txt
for /f "tokens=3 delims=}&"  %%i in (test\inc_temp2.txt) do echo %%i>>test\split_inc2.txt
sed -i 's/\//\\\/g' test\split_inc2.txt
sed -i 's/\\\prj\\\/prj\\\/g' test\split_inc2.txt
sed -i 's/\\\src\\\/src\\\/g' test\split_inc2.txt
sed -i 's/\\\inc\\\/inc\\\/g' test\split_inc2.txt
sed -i 's/\\\lib\\\/lib\\\/g' test\split_inc2.txt
for /f "tokens=*" %%i in (temp.txt) do (
for /f "tokens=*" %%j in (test\split_inc2.txt) do (
echo f|xcopy /y /i %%j\*.h %PRJ_NAME1%_%%i\%PRJ_NAME1%\%%j\*.h
echo f|xcopy /y /i %%j\*.a %PRJ_NAME1%_%%i\%PRJ_NAME1%\%%j\*.a
)
)

@REM<-- Clear src tag information*************************************************************************
::for /f "tokens=*" %%j in (temp.txt) do (
::call %ROOT_PATH%\test\common_script\common_src_clean_tags.bat %ROOT_PATH%\%PRJ_NAME1%_%%j\%PRJ_NAME1%
::)

@REM<-- zip the code folder*****************
for /f "tokens=*" %%i in (temp.txt) do (
zip -r -S %PRJ_NAME1%_%%i.zip %PRJ_NAME1%_%%i
)

@REM<--Copy the folder to the release path*****************
for /f "tokens=*" %%i in (temp.txt) do (
move %PRJ_NAME1%_%%i.zip  %RELEASE_PATH%\%Storage_name%\%Storage_name%_%%i\%PRJ_NAME1%_%%i.zip
xcopy /y /i /s %PRJ_NAME1%_%%i\%PRJ_NAME1%\* %RELEASE_PATH%\%Storage_name%\%Storage_name%_%%i\Source_and_Toolchain\
rd /s /q  %PRJ_NAME1%_%%i
)

@del temp.txt
@del test\*.txt
@call toolchain.bat clean
@REM -->