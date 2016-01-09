@REM <-- Set all the variables********************PLEASE CHANGE***************

@SET OBJ_PLATFORM=mips4k
@SET PRJ_PATH=config_prj\dvbs\warriors\kokanee\demo\kokanee
@SET PRJ_NAME=kokanee
@SET ROOT_PATH=%cd%
@SET PRJ_BINARY_PATH=prj\dvbs\kokanee\binary
@REM <-- **************************call common script*************************************

@call %ROOT_PATH%\test\common_script\common_prj_compile_check.bat %OBJ_PLATFORM% %PRJ_PATH% %PRJ_NAME% %PRJ_BINARY_PATH%