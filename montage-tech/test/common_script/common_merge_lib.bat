@echo off
set wpath=%1
set pname=%2
set platform=%3
@CALL toolchain.bat %platform%
mkdir lib_files
dir /s/b %wpath%\%pname%\obj\*.o >libs.txt
for /f "tokens=*" %%i in (libs.txt) do copy /y %%i lib_files
pushd lib_files
ar cru lib_%pname%.a *.o
copy /y lib_%pname%.a %wpath%\%pname%\release\binary\
popd
copy /y %wpath%\%pname%\post_build.sh %wpath%\%pname%\release\binary\
copy /y %wpath%\%pname%\link.lds %wpath%\%pname%\release\binary\
rd /s /q lib_files
del libs.txt