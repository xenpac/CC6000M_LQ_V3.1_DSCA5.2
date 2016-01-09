@echo off
@setlocal EnableDelayedExpansion
@call toolchain.bat win32
@echo flash usage count, please wait.......
@SET bin_path=%1
@SET flash_size=%2

@pushd %bin_path%
@type flash.cfg > flash.txt
@gawk -F: '/^^^file=/ {print $1}' flash.txt |sed -e 's/file=//g' > file.txt
@gawk -F: '/^^^size=/ {print $1}' flash.txt |sed -e 's/size=\(.*\)K/\1/g' > size.txt
@set size=0
@for /f "tokens=*" %%i in (file.txt) do (
@set /a size+=%%~zi
 )
@set /a size/=1024
:@echo filesize=!size!K
@for /f "tokens=*" %%j in (size.txt) do (
@set /a size+=%%j
 )
@set /a freespace=flash_size*1024-!size!
@echo The following is the space usage report of flash: > flash_usage_report.txt
@echo Total space=%flash_size%M >>flash_usage_report.txt
@echo Used space=!size!K >>flash_usage_report.txt
@echo Free space=!freespace!K  >>flash_usage_report.txt
@del file.txt
@del size.txt
@del flash.txt
@call toolchain.bat clean
@popd 
@echo Done~~
