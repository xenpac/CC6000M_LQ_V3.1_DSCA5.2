
@REM -->/***************************************************************************************************************/
@REM --> script description: check config file(sys_cfg.h¡¢mem_cfg.h) change
@REM -->/***************************************************************************************************************/

@REM<-- Set all the variables*
@SET OBJ_PLATFORM=jazz
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
findstr /i "prj/dvbc/jasmine_demo/includes/memmap_hd_8m_128m/mem_cfg.h prj/dvbc/jasmine_demo/includes/memmap_hd_8m_128m/sys_cfg.h" all_changed_files.txt
if %errorlevel% NEQ 0 goto end
php d:\work\check.php %1

:end
del all_changed_files.txt
del ou_changeset.txt
exit /b 0