@SET OBJ_PATH=%1

@dir /S /B %OBJ_PATH%\*.cbp > %OBJ_PATH%\cbp_files.txt
@Call toolchain.bat mips1
@sed 's/.*montage-tech\\\//g' %OBJ_PATH%\cbp_files.txt > %OBJ_PATH%\cbp_files1.txt
for /f "tokens=1" %%i in (%OBJ_PATH%\cbp_files1.txt) do ( 
type %%i > %%i.txt
sed '/DPRINT_ON/d' %%i.txt > %%i
)
@Call toolchain.bat clean
