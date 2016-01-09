@REM --> Set all the variables
setlocal EnableDelayedExpansion
@SET OBJ_PLATFORM=jazz
@SET BRANCH_NAME=%1
@SET PRJ_NAME=%2
@SET CHECK_PRJ=%3
@SET CURRENT_PATH=%cd%
@SET Lit_path=C:\lint\config
@REM <-- Initialize Environment
@CALL toolchain.bat %OBJ_PLATFORM%
@REM -->
set /a num_b=0

@REM <--Get repository name
for /f "tokens=3 delims=\" %%i in ("%CURRENT_PATH%") do set storage_name=%%i
if /i %CHECK_PRJ% == all goto all
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
REM <-- Get all the changed cfiles in this bundle
@hg st -m -a --rev %changeset% > test\no_delete.txt

@REM <-- Filter file
unix2dos test\no_delete.txt
@findstr ".cpp$ .c$" test\no_delete.txt >test\modify.txt
findstr . test\modify.txt >nul if not %errorlevel% == 0 goto end
for /f "tokens=2 delims= " %%i in (test\no_delete.txt) do echo %%i>>modify_src.txt
if exist test\modify.txt del test\modify.txt
if exist test\no_delete.txt del test\no_delete.txt

@REM <-- get prj
for /f %%i in ('dir /s/b config_prj\%PRJ_NAME%') do set prj_path=%%i
findstr /C:"<Unit filename=" %prj_path% | >prj_src.txt findstr /C:".c\"^>"
for /f "tokens=*" %%i in (modify_src.txt) do (
	findstr /C:"%%i" prj_src.txt
	if !errorlevel! == 0 echo %CURRENT_PATH%\%%i>>%PRJ_NAME%.src.lnt
)
findstr . %PRJ_NAME%.src.lnt >nul if not %errorlevel% == 0 goto end
findstr /c:"<Add directory" %prj_path% >temp.txt
for /f "tokens=3 delims=}&" %%i in (temp.txt) do echo -i%CURRENT_PATH%%%i >>%PRJ_NAME%.lnt
>temp.txt findstr /c:"<Add option=\"-D" %prj_path%
for /f "tokens=3 delims== " %%i in (temp.txt) do echo %%~i >>%PRJ_NAME%.lnt
if exist temp.txt del temp.txt
if exist modify_src.txt del modify_src.txt
if exist prj_src.txt del prj_src.txt

@REM Run pclint check
%Lit_path%\..\LINT-NT.EXE -u -i%Lit_path%\ -os(test\out.txt) -fff std env-codeblock %PRJ_NAME%.lnt %PRJ_NAME%.src.lnt
if exist jasmine_demo_hd.cbp.lnt del jasmine_demo_hd.cbp.lnt
if exist jasmine_demo_hd.cbp.src.lnt del jasmine_demo_hd.cbp.src.lnt
@findstr /c:"error" test\out.txt
if %errorlevel% ==0 goto fail
del test\out.txt
:end
exit /b 0
:fail
exit /b -1

:all
for /f %%i in ('dir /s/b config_prj\%PRJ_NAME%') do set prj_path=%%i
findstr /C:"<Unit filename=" %prj_path% | >prj_src.txt findstr /C:".c\"^>"
findstr /c:"<Add directory" %prj_path% >temp.txt
for /f "tokens=3 delims=}&" %%i in (temp.txt) do echo -i%CURRENT_PATH%%%i >>%PRJ_NAME%.lnt
>temp.txt findstr /c:"<Add option=\"-D" %prj_path%
for /f "tokens=3 delims== " %%i in (temp.txt) do echo %%~i >>%PRJ_NAME%.lnt
call toolchain jazz
sed -i 's/\.\.\\\//g' prj_src.txt
for /f tokens^=2^ delims^=^" %%i in (prj_src.txt) do echo %%i>>prj_src_f.txt
for /f "tokens=*" %%i in (prj_src_f.txt) do echo %CURRENT_PATH%\%%i>>%PRJ_NAME%.src.lnt
%Lit_path%\..\LINT-NT.EXE -u -i%Lit_path%\ -os(test\%PRJ_NAME%_error.log) -fff std env-codeblock %PRJ_NAME%.lnt %PRJ_NAME%.src.lnt
if exist temp.txt del temp.txt
if exist prj_src.txt del prj_src.txt
if exist prj_src_f.txt del prj_src_f.txt
if exist %PRJ_NAME%.lnt del %PRJ_NAME%.lnt
if exist %PRJ_NAME%.src.lnt del %PRJ_NAME%.src.lnt
exit /b 0