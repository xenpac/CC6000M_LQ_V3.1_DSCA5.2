@REM<-- Set all the variables****************please change***************************

@SET OBJ_PLATFORM=win32
@SET PROJECT_PATH=config_prj\dvbc\concerto\jasmine\demo_hd
@SET PROJECT_NAME=jasmine_demo_hd_x86

@REM -->**************************************please do not change****************************

@call test\common_script\common_prj_x86_compile_check.bat %OBJ_PLATFORM% %PROJECT_PATH% %PROJECT_NAME%