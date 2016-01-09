
@REM -->/***************************************************************************************************************/
@REM --> script description: please do not modify the file:test/prjtest_script_txt,test/common_script,config_prj/other/common,
@REM --> test/rule_script,all compile_check bat,all board_config.xls, board_config.bin
@REM -->/***************************************************************************************************************/

@REM<-- Set all the variables*
@SET OBJ_PLATFORM=win32
@REM -->

@REM <-- Initialize Environment
@CALL toolchain.bat %OBJ_PLATFORM%
@REM -->
@REM<-- get all changed release or compile check bat
@echo off
hg ou -M -v > all_files.txt

@REM<--del exception changeset
call D:\work\rels_compile_limit\rels_compile_limit_ex.bat
@REM -->

gawk -F: '/files:/ {print}' all_files.txt >all_files1.txt
gawk 'BEGIN {FS="       "} {print $2}' all_files1.txt >>temp_files.txt
del all_files1.txt
gawk 'BEGIN{RS=" "}{print}' temp_files.txt > changed_files.txt
findstr "test/prjtest_script test/intetest_script test/vctest_script test/release_script_txt" changed_files.txt > limit.txt
findstr "test/c_project test/h_project test/releaseproject .hgignore .hgeol" changed_files.txt >> limit.txt
findstr "test/common_script/ config_prj/other/common/ test/rule_script/ test/rule_script_txt/ test/global_symbol_check" changed_files.txt >> limit.txt
findstr "board_config.xls board_config.bin" changed_files.txt >> limit.txt
:findstr "^config_prj/.*release_.*sdk" changed_files.txt >> limit.txt

@REM -->

@findstr . limit.txt
if %errorlevel%==1 goto nochange

@REM<--remove the repeated files
for /f "delims=" %%i in (limit.txt) do (
if defined %%i set %%i=
)
for /f "delims=" %%i in (limit.txt) do ( 
if not defined %%i set %%i=A & echo %%i>>limitS.txt
) 
type limitS.txt > limit.txt
del limitS.txt
@REM -->

@REM<-- get user or changeset for all changed release or compile check bat
@sed '/^tag:   /d' all_files.txt > all_files1.txt
@sed '/^branch:   /d' all_files1.txt > all_files.txt
@sed '/^parent:   /d' all_files.txt > all_files1.txt
@setlocal enabledelayedexpansion
for /f "tokens=*" %%i in (limit.txt) do (
findstr /n "^files:.*%%i" all_files1.txt |gawk 'BEGIN {FS=":"}{print $1} > number.txt
for /f "tokens=1" %%j in (number.txt) do (
set /a num = %%j
set /a num-=2
sed -n '!num!p' all_files1.txt>> bothModify.txt
echo HAS MODIFIED FILE: %%i IN  >> bothModify.txt
set /a num-=1
sed -n '!num!p' all_files1.txt>> bothModify.txt
)
)

@REM -->
@REM <-- except files modified by sqa member
test\rels_compile_limit\rels_compile_limit.exe
if %errorlevel%==0 goto change
for /f "tokens=1" %%i in (user.txt) do (
sed '/%%i/,/changeset:   /d' bothModify.txt > limitM.txt
type limitM.txt > bothModify.txt
)
del user.txt
@REM -->

@findstr /C:"HAS MODIFIED FILE:" limitM.txt
if %errorlevel%==1 goto nochangeEx
type limitM.txt > test\out.txt


:change
del all_files.txt
del temp_files.txt
del changed_files.txt
del limit.txt
del limitM.txt
del all_files1.txt
del number.txt
del bothModify.txt
@CALL toolchain.bat clean
@echo on
@echo   you can find this script description in test\rule_script\rels_compile_limit.bat>>test\out.txt
exit /b -1

:nochangeEx
del limitM.txt
del all_files1.txt
del number.txt
del bothModify.txt

:nochange
del all_files.txt
del changed_files.txt
del temp_files.txt
del limit.txt
@CALL toolchain.bat clean
@echo on
exit /b 0

