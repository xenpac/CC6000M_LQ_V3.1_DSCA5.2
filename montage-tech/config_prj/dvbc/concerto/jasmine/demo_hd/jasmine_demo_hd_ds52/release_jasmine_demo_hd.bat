@REM <-- Set all the variables********************PLEASE CHANGE***************
@SET OBJ_PLATFORM=mips24k
@SET COMMON_RELEASE=test\common_script\common_prj_release_core_zip.bat
@SET CORE_PATH=config_prj\core\core_jasmine
@SET CORE_CBP_NAME=core_jasmine.workspace
@SET LIB_RELEASE_PATH=\\192.168.8.32\upload\release\prj_lib
@SET ROOT_PATH=%cd%
@SET PRJ_PATH=config_prj\dvbc\concerto\jasmine\demo_hd
@SET PRJ_NAME=jasmine_demo
@SET PRJ_CFG_NAME=jasmine_demo_hd
@SET PRJ_CUSTOMER_NAME=demo_hd
@SET PRJ_BINARY_PATH="%ROOT_PATH%\prj\dvbc\%PRJ_NAME%\binary %ROOT_PATH%\prj\dvbc\%PRJ_NAME%\customer\%PRJ_CUSTOMER_NAME%\%PRJ_CFG_NAME%\binary"
@SET CHIP=CONCERTO
@SET FP=demo
@SET RELEASE_PATH=\\192.168.8.32\upload\release
@SET RELEASE_PATH=.\
@SET PRINT_CORE=off
@SET PRINT_PRJ=off
SET PRJ_RELEASE_TXT=release_src_for_ic.txt
@SET LIB_RELEASE=test\common_script\common_prj_release_for_lib.bat
@SET link_compile=test\common_script\Link_compile_check.bat
@REM<-----flash_size: flash size number with the unit MB ( e.g: flash_size=4 means 4M, flash_size=0.4 means 0.4M)
@SET flash_size=4
@REM <-- CALL COMMON BATS*********************************************

@REM <--Copy warriors_privlib*******************
:@copy /Y \\192.168.8.32\driver_report\warriors_privlib\Flounder_tongjiu_Turkey\libcore_jasmine_drv.a lib\lib_concerto\
:@copy /Y \\192.168.8.32\driver_report\warriors_privlib\Flounder_tongjiu_Turkey\libcore_jasmine_drv.a config_prj\core\core_jasmine
@REM ->

@REM <--CALL THE BAT TO OFF PRINT
@if %PRINT_CORE%==off call test\common_script\common_core_print_off.bat %CORE_PATH%
@if %PRINT_PRJ%==off call test\common_script\common_core_print_off.bat %PRJ_PATH%\%PRJ_CFG_NAME%

@REM <-- CALL THE COMMON RELEASE SCRIPT
@call %COMMON_RELEASE% %OBJ_PLATFORM% %CORE_PATH% %CORE_CBP_NAME%  %PRJ_PATH% %PRJ_CFG_NAME% %CHIP% %FP% %RELEASE_PATH% %PRJ_BINARY_PATH% %1
@call %LIB_RELEASE% %OBJ_PLATFORM% %PRJ_PATH% %PRJ_CFG_NAME% %LIB_RELEASE_PATH% %PRJ_BINARY_PATH% %PRJ_RELEASE_TXT%
@call %link_compile% %OBJ_PLATFORM%
@REM <-- COUNT FLASH USAGE
@call toolchain.bat win32
@hg parent | gawk -F: '/changeset:/ {print $2}' |sed -e 's/ //g' > temp.txt
@for /f "tokens=*" %%i in (temp.txt) do (
SET bin_path=%RELEASE_PATH%\%PRJ_CFG_NAME%\%PRJ_CFG_NAME%_%%i\%PRJ_CFG_NAME%_%%i_binary
SET version=%%i
)
@call test\common_script\common_prj_flash_usage_count.bat  %bin_path% %flash_size%
@del temp.txt

@REM --> Revert core cbp files
if %PRINT_CORE%==off for /f "tokens=1" %%i in (%CORE_PATH%\cbp_files1.txt) do ( 
type %%i.txt > %%i
del %%i.txt
)
if %PRINT_PRJ%==off for /f "tokens=1" %%i in (%PRJ_PATH%\%PRJ_CFG_NAME%\cbp_files1.txt) do ( 
type %%i.txt > %%i
del %%i.txt
)
SET RELEASE_PATH=\\192.168.8.32\upload\release\Jasmine\Jasmine_%version%
mkdir %RELEASE_PATH%
echo D|xcopy /y /s %PRJ_CFG_NAME% %RELEASE_PATH%
@del %CORE_PATH%\cbp_files*.txt
@del %PRJ_PATH%\%PRJ_CFG_NAME%\cbp_files*.txt
