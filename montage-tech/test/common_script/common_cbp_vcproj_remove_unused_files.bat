@echo off 
set cbp_dir=%1
set vcproj_dir=%2
set refer_file=%3

call toolchain.bat jazz
sed 's/\\\/\\\\\\\/g' %refer_file% >file_need.txt
findstr /C:".c" file_need.txt >c_file_need.txt
findstr /C:".h" file_need.txt >h_file_need.txt


REM -----Remove Unused Files From Cbp Files
dir /S /B %cbp_dir%\*.cbp > cbp_files.txt
sed 's/.*montage-tech\\\//g' cbp_files.txt > cbp_files1.txt
for /f "tokens=1" %%i in (cbp_files1.txt) do ( 
findstr /C:".c" %%i >cfile_exist.txt
findstr  /v /g:c_file_need.txt  cfile_exist.txt >cfile_del0.txt
sed 's/\\\/\\\\\\\\\\\/g' cfile_del0.txt >cfile_del.txt
for /f "tokens=*" %%j in (cfile_del.txt ) do (
sed -i '/%%j/,/\/\Unit/d' %%i
)
findstr /C:".h" %%i >hfile_exist.txt
findstr  /v /g:h_file_need.txt hfile_exist.txt >hfile_del0.txt
sed 's/\\\/\\\\\\\\\\\/g' hfile_del0.txt >hfile_del.txt
for /f "tokens=2" %%j in (hfile_del.txt ) do (
sed -i '/%%j/d' %%i
)
)

REM -----Remove Unused Files From Vcproj Files
@setlocal enabledelayedexpansion
dir /S /B %vcproj_dir%\*.vcproj > vcproj_files.txt
sed 's/.*montage-tech\\\//g' vcproj_files.txt > vcproj_files1.txt
for /f "tokens=1" %%i in (vcproj_files1.txt) do ( 
findstr /C:".c" %%i >file_exist.txt
findstr /C:".h" %%i >>file_exist.txt
findstr  /v /g:file_need.txt file_exist.txt >file_del0.txt
sed 's/.*RelativePath="..\\\\..\\\\..\\\\\(.*\)"/\1/g' file_del0.txt >file_del1.txt
sed 's/\\\/\\\\\\\/g' file_del1.txt >file_del.txt
for /f "tokens=*" %%j in (file_del.txt ) do (
findstr /n /C:"%%j" %%i |gawk 'BEGIN {FS=":"}{print $1} > number.txt
for /f "tokens=1" %%k in (number.txt) do (
set /a num_start=%%k-1
set /a num_end=%%k+2
sed -i '!num_start!,!num_end!d' %%i
)
)
)
call toolchain.bat clean

del *file*.txt
del number.txt

