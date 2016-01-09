
@REM -->/***************************************************************************************************************/
@REM --> script description: do not allow use string "os_printk" in prj,tools,src,inc,lib,except file:mtos_printk.c,mtos_printk.h,ap_init.c,log_def.h 
@REM -->/***************************************************************************************************************/

@REM<-- Set all the variables*
@SET OBJ_PLATFORM=win32
@REM -->

@REM <-- Initialize Environment
@CALL toolchain.bat %OBJ_PLATFORM%
@REM -->

@REM <-- Get changed files
hg ou -v >ou_changeset.txt
gawk -F: '/files:/ {print}' ou_changeset.txt >temp.txt
gawk 'BEGIN {FS="       "} {print $2}' temp.txt >changed_files.txt
gawk 'BEGIN{RS=" "}{print}' changed_files.txt >all_changed_files.txt
del changed_files.txt
@REM -->

@REM <-- Findstr in these files
findstr /R /v "^config ^test mtos_printk.c mtos_printk.h ap_init.c log_def.h ucos_msg.c sys_app.c" all_changed_files.txt>changed_files.txt
del all_changed_files.txt
findstr /I /P /N /C:"OS_PRINTK" /F:changed_files.txt >test\changed_files.txt
findstr /I /v /C:"//" test\changed_files.txt >test\changed_files1.txt
findstr /I /v /C:"mtos_printk.h" test\changed_files1.txt >test\changed_files2.txt
findstr . test\changed_files2.txt
if not %errorlevel%==0 goto end
@REM -->

@REM <-- Return result
echo "OS_PRINTK NOT ALLOWED!!!!!!!!!!!!!!!!">test\out.txt
type test\changed_files2.txt>>test\out.txt
del ou_changeset.txt
del temp.txt
del test\changed_files.txt
del test\changed_files1.txt
del test\changed_files2.txt
del changed_files.txt
@echo   you can find this script description in test\rule_script\print_limit.bat>>test\out.txt
exit /b -1
:end
del ou_changeset.txt
del temp.tx
del test\changed_files.txt
del test\changed_files1.txt
del test\changed_files2.txt
del changed_files.txt
del test\out.txt
exit /b 0