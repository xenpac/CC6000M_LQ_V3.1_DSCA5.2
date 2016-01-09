@SET OBJ_PATH=%1

@if exist %OBJ_PATH% (
call toolchain.bat jazz
dir /S /B %OBJ_PATH%\*.c %OBJ_PATH%\*.cpp %OBJ_PATH%\*.h %OBJ_PATH%\*.hpp %OBJ_PATH%\*.ld > all_src_files.txt
dos2unix all_src_files.txt
dos2unix test\common_script\common_src_clean_tags.sh
sh test\common_script\common_src_clean_tags.sh
del all_src_files.txt

dir /S /B %OBJ_PATH%\*.a %OBJ_PATH%\*.lib > all_lib_files.txt
for /f "tokens=1" %%i in (all_lib_files.txt) do (
test\lib_tag_clear\lib_tag_clear.exe -p %%i.a %%i
type %%i.a  > %%i
)
del all_lib_files.txt
)
del /f /s /q %OBJ_PATH%\*.a.a
del /f /s /q %OBJ_PATH%\*.lib.a
