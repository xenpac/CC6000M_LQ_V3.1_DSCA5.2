@REM <-- Set some variables**********************PLEASE CHANGE*************************
@SET RELEASE_PATH=\\192.168.8.32\upload\release
@SET ROOT_PATH=%cd%
@SET OBJ_PLATFORM=mips24k

@SET PRJ_PATH=config_prj\dvbc\concerto\jasmine\demo_hd\jasmine_demo_hd
@SET PRJ_NAME=jasmine_demo
@SET PRJ_COPY_TXT=jasmine_demo_hd_sdk_release_src.txt
@SET PRJ_PATCH_TXT=jasmine_demo_hd_sdk_patch.txt
@SET PRJ_BIN_PATH=prj\dvbc\jasmine_demo\binary
@SET SRC_PATH=\\192.168.8.32\upload\build_work\warden\SDK\Flounder\Flounder_SDK_2.0

@REM -->
@SET PRINT_PRJ=off
@REM <-- CALL COMMON BATS*********************************************
hg purge --all
@REM <--CALL THE BAT TO OFF PRINT
@if %PRINT_PRJ%==off call test\common_script\common_core_print_off.bat %PRJ_PATH%\%PRJ_NAME%

@REM<-- Set the toolchain*****************
call toolchain.bat %OBJ_PLATFORM%
@REM<-- Get changeset*****************
hg parent | gawk -F: '/changeset:/ {print $2}' |sed -e 's/ //g'>temp.txt

@REM <--Copy release product---you can copy files you any want*******************************************

for /f "tokens=*" %%i in (temp.txt) do (
for /f "tokens=*" %%x in (%PRJ_PATH%\%PRJ_COPY_TXT%) do (
echo f|xcopy /y /i /s %%x %PRJ_NAME%_%%i\Demo\%%x
)
for /f "tokens=*" %%x in (%PRJ_PATH%\%PRJ_PATCH_TXT%) do (
echo f|xcopy /y /i /s %SRC_PATH%\Source_and_Toolchain\%%x %PRJ_NAME%_%%i\Source_and_Toolchain\%%x
)
del %PRJ_NAME%_%%i\Demo\%PRJ_PATH%\%PRJ_COPY_TXT%
del %PRJ_NAME%_%%i\Demo\%PRJ_PATH%\%PRJ_PATCH_TXT%
del %PRJ_NAME%_%%i\Demo\%PRJ_PATH%\release_%PRJ_NAME%.bat
del %PRJ_NAME%_%%i\Demo\%PRJ_PATH%\release_%PRJ_NAME%_sdk.bat
)
:@REM <--Copy warriors_privlib*******************
:for /f "tokens=*" %%i in (temp.txt) do (
:copy /Y \\192.168.8.32\driver_report\warriors_privlib\Flounder_demo\libdummy_fake.a %PRJ_NAME%_%%i\Source_and_Toolchain\lib\lib_warriors\
:)
@REM ->

@REM -->

@REM --> Revert core cbp files
for /f "tokens=*" %%x in (temp.txt) do (
if %PRINT_PRJ%==off for /f "tokens=1" %%i in (%PRJ_PATH%\%PRJ_NAME%\cbp_files1.txt) do ( 
type %%i.txt > %%i
del %%i.txt
del %PRJ_NAME%_%%x\Demo\%%i.txt
)
del %PRJ_NAME%_%%x\Demo\%PRJ_PATH%\%PRJ_NAME%\cbp_files*.txt
del %PRJ_PATH%\%PRJ_NAME%\cbp_files*.txt
)

@REM<-- Clear src tag information*************************************************************************
@REM<-- Clear src tag information*************************************************************************
for /f "tokens=*" %%j in (temp.txt) do (
call %ROOT_PATH%\test\common_script\common_src_clean_tags.bat %ROOT_PATH%\%PRJ_NAME%_%%j\Demo
)
@REM<-- zip the code folder*****************
for /f "tokens=*" %%i in (temp.txt) do (
zip -r -S %PRJ_NAME%_%%i.zip %PRJ_NAME%_%%i
mkdir %RELEASE_PATH%\%PRJ_NAME%\%PRJ_NAME%_%%i
)
@REM -->

@REM<--Copy the folder to the release path*****************
for /f "tokens=*" %%i in (temp.txt) do (
move %PRJ_NAME%_%%i.zip  %RELEASE_PATH%\%PRJ_NAME%\%PRJ_NAME%_%%i\%PRJ_NAME%_%%i.zip
rd /s /q  %PRJ_NAME%_%%i
)
@del temp.txt
@call toolchain.bat clean
@REM -->