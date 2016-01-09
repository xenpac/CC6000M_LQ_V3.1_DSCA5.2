@REM <-- Set all the variables********************PLEASE CHANGE***************

@SET OBJ_PLATFORM=mips1
@SET PRJ_PATH=config_prj\dvbc\concerto\jasmine\demo_hd\FastLogoAp
@SET PRJ_NAME=FastLogoAp
@SET ROOT_PATH=%cd%
@SET AV_MEM_CONFIG=MIN_AV_128M
@SET AV_SD_HD_CONFIG=MIN_AV_SD_HD
@SET PRJ_BINARY_PATH=prj/dvbc/jasmine_demo/binary
@REM <-- **************************call common script*************************************

@call %ROOT_PATH%\test\common_script\common_prj_compile_check.bat %OBJ_PLATFORM% %PRJ_PATH% %PRJ_NAME% %PRJ_BINARY_PATH%