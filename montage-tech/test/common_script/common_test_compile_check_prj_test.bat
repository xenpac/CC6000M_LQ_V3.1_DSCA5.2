@REM -->******************************script usage as below:*****************************
::        common_test_compile_check_config_test.bat PRJ_NAME HW_PLATFORM TEST_MODE     ::                     
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

@SET PRJ_NAME=%1
@SET HW_PLATFORM=%2
@SET TEST_MODE=%3
if exist %ROOT_PATH%\*.log del /q *.log
for %%i in (config_prj config_test) do dir /s/b %%i\%PRJ_NAME% >>temp.log
set /p FILE_PATH=<temp.log
for /f "delims=" %%i in ("%FILE_PATH%") do (
set WORKSPACE_PATH=%%~dpi
set CORE_NAME=%%~ni
)
findstr "compiler=" %WORKSPACE_PATH%\%PRJ_NAME% >platform.log
for /f tokens^=2^ delims^=^" %%i in (platform.log) do set OBJ_PLATFORM=%%i
findstr /C:"Target title=" %WORKSPACE_PATH%\%PRJ_NAME% >prjconfig.log
for /f tokens^=2^ delims^=^" %%i in (prjconfig.log) do set PRJ_CONFIG=%%i

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



@SET LIB_NAME=lib%CORE_NAME%.a
@SET LIB_DEST=Y:\libcore\%DES_NAME%\
@REM -->

::@REM <-- REM Initialize Environment
::@SET WORKSPACE_PATH=%ROOT_PATH%\%PRJ_PATH%

@CALL toolchain.bat %OBJ_PLATFORM%
@if %OBJ_PLATFORM%==jazz dos2unix %WORKSPACE_PATH%\post_build.sh
@REM -->

@REM <-- Load project
@REM STEP 1: Initialize project variables


@REM STEP 2: Cleanup environment
::@if exist %WORKSPACE_PATH%\bin rd /Q /S %WORKSPACE_PATH%\bin
@if exist %WORKSPACE_PATH%\obj rd /Q /S %WORKSPACE_PATH%\obj

@REM STEP 3: Import & compile project
if exist %WORKSPACE_PATH%\%PRJ_NAME% codeblocks.exe --no-splash-screen --rebuild %WORKSPACE_PATH%\%PRJ_NAME% --target=Release >test\out.txt


@REM <--  Cleanup environment
@if exist %WORKSPACE_PATH%\.objs rd /Q /S %WORKSPACE_PATH%\.objs
@if exist %WORKSPACE_PATH%\obj rd /Q /S %WORKSPACE_PATH%\obj
@if exist %WORKSPACE_PATH%\%PRJ_CONFIG% rd /Q /S %WORKSPACE_PATH%\%PRJ_CONFIG%
@if exist %WORKSPACE_PATH%\*.layout del %WORKSPACE_PATH%\*.layout
@if exist %WORKSPACE_PATH%\*.depend del %WORKSPACE_PATH%\*.depend
@CALL toolchain.bat clean
@REM -->
if "%5"=="" goto normal

:ext
@findstr /R /i /v /g:test\prjtest_script_txt\%PRJ_EXCEPT_WARNING%_warning_exception.txt test\out.txt >test\outtemp.txt
@copy /Y test\outtemp.txt test\out.txt
@if exist test\outtemp.txt del test\outtemp.txt

:normal
@REM <--  Return the compile result
@findstr /B /C:"0 errors" test\out.txt
@if not %errorlevel%==0 goto bad
@findstr /C:": error:" test\out.txt
@if %errorlevel%==0 goto bad
@findstr /C:"No such file or directory" test\out.txt
@if %errorlevel%==0 goto bad
@findstr /C:"Process terminated with status 1" test\out.txt
@if %errorlevel%==0 goto bad
@del test\out.txt
del /q *.log
if not "%CORE_NAME%"=="" copy /Y %WORKSPACE_PATH%\bin\Release\%LIB_NAME% %LIB_DEST%
::@if exist %WORKSPACE_PATH%\bin rd /Q /S %WORKSPACE_PATH%\bin
exit /b 0

:bad
@echo **********************************************************************************>>test\out.txt
@echo The compile failed project path is:>>test\out.txt
@echo %WORKSPACE_PATH%\%PRJ_NAME%>>test\out.txt
@type test\prjtest_script_txt\errorlog_files\compile_check_error_log.txt>>test\out.txt
::@if exist %WORKSPACE_PATH%\bin rd /Q /S %WORKSPACE_PATH%\bin
exit /b -1
@REM -->