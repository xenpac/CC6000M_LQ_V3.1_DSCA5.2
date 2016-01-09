@REM<-- Set all the variables****************please change***************************

@SET OBJ_PLATFORM=%1
@SET PROJECT_PATH=%2
@SET PROJECT_NAME=%3

@REM -->**************************************please do not change****************************

@REM <-- Initialize Environment
@CALL toolchain.bat %OBJ_PLATFORM%
@REM -->

@REM <--**************************************check if contain x86_prebuild****************************
@if exist %PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.vcproj type %PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.vcproj > prj_x86.txt
@sed -n '/RelativePath=.*\.\.\\\\src\\\\/p' prj_x86.txt > limit.txt
@sed -n '/RelativePath=.*\.\.\\\\inc\\\\/p' prj_x86.txt >> limit.txt
@del prj_x86.txt
@findstr /C:"x86_prebuild_lib.vcproj" %PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.sln
@if %errorlevel%==0 echo Attention:Please remove x86_prebuild_lib.vcproj from your prj_x86.sln file!!!!! >> limit.txt
@findstr . limit.txt>nul || goto pass0
@goto bad0
:pass0
@if exist limit.txt del limit.txt
@REM -->

@REM <-- Build project
@call test\common_script\test_win32_prj  %PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.sln /rebuild debug>test\out.txt
@findstr /C:"0 failed, 0 skipped" test\out.txt
@if not %errorlevel%==0 goto bad
@del test\out.txt
@if exist %PROJECT_PATH%\%PROJECT_NAME%\Debug rd /s /q %PROJECT_PATH%\%PROJECT_NAME%\Debug
@CALL toolchain.bat clean
exit /b 0
:bad0
@echo ----You have added core files in your prj_x86.vcproj file > test\out.txt
@echo -----or you have added x86_prebuild_lib.vcproj in your prj_x86.sln: >> test\out.txt
@type limit.txt >> test\out.txt
@del limit.txt
:bad
@if exist %PROJECT_PATH%\%PROJECT_NAME%\Debug rd /s /q %PROJECT_PATH%\%PROJECT_NAME%\Debug
@CALL toolchain.bat clean
exit /b -1