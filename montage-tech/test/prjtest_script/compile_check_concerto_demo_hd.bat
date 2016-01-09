@REM <-- Set all the variables********************PLEASE CHANGE***************

@SET OBJ_PLATFORM=mips4k
@SET PRJ_PATH=config_prj\dvbc\concerto\jasmine\demo_hd\jasmine_demo_hd
@SET PRJ_NAME=jasmine_demo_hd
@SET ROOT_PATH=%cd%
@SET PRJ_BINARY_PATH=prj/dvbc/jasmine_demo/binary
@REM <-- **************************call common script*************************************

@call %ROOT_PATH%\test\common_script\common_prj_compile_check.bat %OBJ_PLATFORM% %PRJ_PATH% %PRJ_NAME% %PRJ_BINARY_PATH%