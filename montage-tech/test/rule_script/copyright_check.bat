@REM --> Set all the variables
@SET OBJ_PLATFORM=jazz
@SET BRANCH_NAME=%1
@SET CURRENT_PATH=%cd%

@REM <-- Initialize Environment
@CALL toolchain.bat %OBJ_PLATFORM%
@REM -->
set /a num_b=0

@REM <--Get repository name
for /f "tokens=3 delims=\" %%i in ("%CURRENT_PATH%") do set storage_name=%%i
 
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
:deal
@REM <-- Get all the changed cfiles in this bundle
@hg st -a --rev %changeset% > test\no_delete.txt
findstr . test\no_delete.txt >nul
if not %errorlevel% == 0 goto end
for /f "tokens=2 delims= " %%i in (test\no_delete.txt) do echo %%i>>test\split.txt
if exist test\no_delete.txt del test\no_delete.txt

REM <-- Filter src files
@findstr /R /B /v /g:test\rule_script_txt\copyright_exception.txt test\split.txt >test\exception.txt
if exist test\split.txt del test\split.txt
@unix2dos test\exception.txt
findstr /e "\.c$ \.cpp$ \.h$ \.hpp$ \.ld$" test\exception.txt >test\filter.txt
findstr . test\filter.txt >nul
if not %errorlevel% == 0 goto end
if exist test\exception.txt del test\exception.txt
@REM <-- Check copyright information in src files
for /f %%i in (test\filter.txt) do (
findstr /C:"Montage Technology (Shanghai) Co., Ltd." %%i || echo %%i not matching copyright :"Montage Technology (Shanghai) Co., Ltd.">>test\out.txt
findstr /C:"Montage Proprietary and Confidential" %%i || echo %%i not matching copyright :"Montage Proprietary and Confidential">>test\out.txt
findstr /C:"Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies" %%i || echo %%i not matching copyright :"Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies">>test\out.txt
)
if exist test\filter.txt del test\filter.txt
findstr . test\out.txt >nul
if %errorlevel% == 0 goto fail

:end
exit /b 0

:fail
exit /b -1