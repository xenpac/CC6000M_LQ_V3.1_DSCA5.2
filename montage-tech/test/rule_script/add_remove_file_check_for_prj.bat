
@REM -->/***************************************************************************************************************/
@REM --> script description: if you want to add or remove files in config_prj/config_test/tools/src/inc/test/lib
@REM --> prj\dvbs\.*\binary files which are not belong to test\rule_script_txt\add_remove_binary_except.txt,please go the add/remove files circulation in cryptlord.
@REM -->/***************************************************************************************************************/


@REM<-- Set all the variables
@SET OBJ_PLATFORM=jazz
@SET BRANCH_NAME=default
@REM <-- Initialize Environment
@CALL toolchain.bat %OBJ_PLATFORM%
@REM -->
set /a num_b=0
@REM <-- Get the head num of the branch before this bundle
hg ou -q -b %BRANCH_NAME% > ou_changeset.txt
@findstr . ou_changeset.txt >nul||goto end
@setlocal enabledelayedexpansion
if %BRANCH_NAME%==default (
sed -n '$=' ou_changeset.txt > ou_number.txt
for /f "tokens=*" %%i in (ou_number.txt) do (
set /a num = %%i+1
)
hg ou -b %BRANCH_NAME% > ou_changeset1.txt
findstr /b /C:"branch:   " ou_changeset1.txt >ou_changeset2.txt
sed -n '$=' ou_changeset2.txt > ou_number2.txt
for /f "tokens=*" %%i in (ou_number2.txt) do (
set /a num_b = %%i
)
set /a num_ou = !num!-!num_b!
) else (
hg ou -b %BRANCH_NAME% > ou_changeset1.txt
findstr /b /C:"branch:      %BRANCH_NAME%" ou_changeset1.txt >ou_changeset2.txt
@sed -n '$=' ou_changeset2.txt > ou_number2.txt
@for /f "tokens=*" %%i in (ou_number2.txt) do (
set /a num_b = %%i
)
set /a num_ou = !num_b!+1
)
@del ou_*.txt

@hg log -q -l %num_ou% -b %BRANCH_NAME% > all_changeset.txt
@sed -n '$=' all_changeset.txt >all_num.txt
@for /f "tokens=*" %%i in (all_num.txt) do (
set /a num_log = %%i
)
@if  not "%num_log%" == "%num_ou%" goto new_branch
:normal
@sed -n '%num_ou%p' all_changeset.txt > last_changeset.txt
@gawk -F: '/(.*):/ {print $1}' last_changeset.txt > last_changeset_num.txt
@for /f "tokens=*" %%i in (last_changeset_num.txt) do (
set changeset=%%i
)
@del all_*.txt
@del last_changeset*.txt
@goto deal

:new_branch
@hg ou -q -l 1 -b %BRANCH_NAME% > bundle_changeset.txt
@gawk -F: '/(.*):/ {print $2}' bundle_changeset.txt > first_changeset.txt
@del bundle_changeset.txt
@for /f "tokens=*" %%i in (first_changeset.txt) do (
set changeset=%%i
)
@del first_changeset.txt
@hg parent -q --rev %changeset% > parent.txt
@gawk -F: '/(.*):/ {print $1}' parent.txt > parent_num.txt
@for /f "tokens=*" %%i in (parent_num.txt) do (
set changeset=%%i
)
@del all_changeset.txt
@del parent.txt
@del parent_num.txt
@REM -->

:deal
@REM <-- Get all the changed cfiles in this bundle
@hg st -a -r --rev %changeset% -X src -X inc > test\no_delete.txt
@del test\out.txt
@REM -->
@REM <-- Filter files
@REM step1 filter prj binary files
@findstr /B "prj.dvbs.*.binary" test\no_delete.txt > bin.txt
@if not %errorlevel%==0 (del bin.txt && goto step2)
@unix2dos bin.txt
@echo . >>bin.txt
@findstr "img$ bin$ cfg$" bin.txt > bin2.txt
@if not %errorlevel%==0 (del bin2.txt && goto step2)
@findstr /v /g:test\rule_script_txt\add_remove_binary_except.txt bin2.txt > bin3.txt

@REM step2 filter other files
:step2
@sed 's/\\\/\\\\\\\\\\\/g' test\efile.txt > test\exception.txt
@sed 's/$/.*/g' test\exception.txt > exception.txt
@del test\exception.txt
@findstr /R /B /v /g:exception.txt test\no_delete.txt>test\temp.txt

@REM step3 put them together
if exist bin3.txt (type bin3.txt >> test\temp.txt && del bin3.txt)
@hg st -a --rev %changeset% src inc > test\src_add.txt
@type test\src_add.txt >> test\temp.txt
@del test\src_add.txt
findstr . test\temp.txt
if not %errorlevel%==0 goto end
@sed 's/^A /You have added the file: /' test\temp.txt >> test\temp_out.txt
@sed 's/^R /You have removed the file: /' test\temp_out.txt > test\out.txt
del exception.txt

del test\no_delete.txt
del test\temp_out.txt
del test\temp.txt
@REM <-- Return the result
@CALL toolchain.bat clean
@echo  you can find this script description in test\rule_script\add_remove_file_check.bat>>test\out.txt
exit /b -1
:end
@CALL toolchain.bat clean
del ou_changeset.txt
del exception.txt
del test\no_delete.txt
del test\temp.txt
exit /b 0
@REM -->
