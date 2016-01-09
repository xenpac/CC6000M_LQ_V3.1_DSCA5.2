@REM <-- Set all the variables

@SET WORKSPACE_NAME=core_jasmine.workspace
@SET DEST_CORENAME=core_jasmine
@SET CHIP=concerto
@SET MACRO_CHIP=CONCERTO
@SET MACRO_WIFI=DISABLE_WIFI
@SET SECURE=ENABLE_ADVANCEDCA_CIPHERENGINE

@REM <-- Create shell & Run background
@echo off
@if not "%1" == "h" mshta vbscript:createobject("wscript.shell").run("%~nx0 h",0)(window.close)&&exit
@REM -->


@REM <-- Load project
codeblocks.exe  %WORKSPACE_NAME%
@REM -->
