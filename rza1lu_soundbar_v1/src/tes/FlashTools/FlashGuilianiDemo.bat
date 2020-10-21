@echo off

rem <> Manually set path to JLink install directory here if you do not
rem    want to use the auto detect method. Make sure a backslash
rem    is at the end of the path
set BASE=C:\Program Files (x86)\SEGGER\JLink_V500k\
if exist "%BASE%\JLink.exe" goto PATH_SET

rem <> Try to automatically detect JLink install directory
set KEYNAME=HKCU\Software\SEGGER\J-Link
set VALNAME=InstallPath
rem Check if JLink is installed first
reg query %KEYNAME% /v %VALNAME%
if not "%ERRORLEVEL%" == "0" (goto NO_PATH)
rem Query the value and then pipe it through findstr in order to find the matching line that has the value.
rem Only grab token 3 and the remainder of the line. %%b is what we are interested in here.
for /f "tokens=2,*" %%a in ('reg query %KEYNAME% /v %VALNAME% ^| findstr %VALNAME%') do (
    set BASE=%%b
)
if exist "%BASE%\JLink.exe" goto PATH_SET

:NO_PATH
chgclr 0C
echo ===================================================================
echo ERROR: You need to set the path for JLink.exe 
echo ===================================================================
pause
chgclr 07
exit
:PATH_SET

rem <> extract the version number from the path
set MINJVER=V600
set JVER=%BASE:~-5%
set JVER=%JVER:~0,-1%
rem Remove '_' if present
set JVER=%JVER:_V=V%
echo Your JLINK Version is %JVER%
echo Minimum JLINK Version is %MINJVER%
echo.
if /I %JVER% GEQ %MINJVER% (goto JTAGCONF_CHECK)
chgclr 0C
echo ===================================================================
echo ERROR: You need at least JLINK verison %MINJVER%
echo ===================================================================
pause
chgclr 07
exit

:JTAGCONF_CHECK

:loop

echo. 
echo                               8888 8888 8888                              8888 
echo                                    8888                                        
echo      8888  8888 8888     8888 8888 8888 8888   888888888   88888  88888   8888 
echo    888888888888 8888     8888 8888 8888 8888 8888888888888 8888888888888  8888 
echo   88888   88888 8888     8888 8888 8888 8888          8888 88888    88888 8888 
echo   8888     8888 8888     8888 8888 8888 8888  888888888888 88888    88888 8888 
echo   888888  88888 8888     8888 8888 8888 8888 8888    88888 88888    88888 8888 
echo    888888888888 8888888888888 8888 8888 8888 8888888888888 88888    88888 8888 
echo            8888   888888 8888 8888 8888 8888  8888888 8888 88888    88888 8888 
echo   88888   8888                                                                 
echo     888888888               B Y  T E S  E L E C T R O N I C  S O L U T I O N S                                                                 
echo  ***************************************************************************** 
echo.
echo Please select one of the following options:
echo. 
echo  1 - Flash QSPI bootloader
echo  3 - Flash Guiliani Resources
echo. 
echo  x - Exit
echo.
set /p SELECTED=Your choice: 

if "%SELECTED%" == "x" goto :eof
if "%SELECTED%" == "1" goto :flashBootloader
if "%SELECTED%" == "3" goto :flashResources
goto :errorInput 

:flashBootloader
cls
echo WARNING: Please make sure that you select the correct bootloader and connect the J-Link Lite Probe with the appropriate board.
echo Choosing incorrect option and hence flashing incorrect bootloader may damage your board.
echo.
pause
:Bootloader_Selection
echo.
echo Which bootloader to flash?
echo Bootloader for StreamIt    = 2
echo Go to main menu            = x
set /p INPUT=
if %INPUT% == 2 goto StreamIt
if %INPUT% == x goto loop
echo Incorrect input & goto Bootloader_Selection
:DisplayIt
echo.
echo NOTE:
echo  Remove power (5V) to the board before continuing. 
echo  Set SW6 as instructed below:
echo  SW6-1 OFF, SW6-2 ON, SW6-3 OFF, SW6-4 ON, SW6-5 ON, SW6-6 ON
echo  Reconnect power (5V) to the board before continuing.
echo.
pause
:StreamIt
echo.
echo You are about to flash bootloader for StreamIt, do you want to proceed? (y/n)
set /p INPUT=
if %INPUT% NEQ y goto Bootloader_Selection
cd JLink
"%BASE%\JLink.exe" -if JTAG -speed 12000 -device R7S721031 -jtagconf -1,-1 -CommanderScript FlashBootloader_StreamIt.Command
pause
cd ..
goto :loop

:flashApplication
echo.
echo Which binary to flash?
echo Release version for StreamIt built with eGML Library         = 2
echo Go to main menu                                              = x
set /p INPUT=
if %INPUT% == 2 goto Release_StreamIt_eGML
if %INPUT% == x goto loop
echo Incorrect input & goto flashApplication
:Release_StreamIt_eGML
echo.
pause
cd JLink
cls
"%BASE%\JLink.exe" -if JTAG -speed 12000 -device R7S721031 -jtagconf -1,-1 -CommanderScript FlashApplication_Release_StreamIt_eGML.Command
pause
cd ..
goto :loop

:flashResources
echo.
echo For which board are resources to be flashed?
echo StreamIt         = 2
echo Go to main menu  = x
set /p INPUT=
if %INPUT% == 2 goto StreamIt
if %INPUT% == x goto loop
echo Incorrect input & goto flashResources
:StreamIt
if not exist "..\GUI_Sample\Include\GUIConfig\Resources.dat" goto :errorNoResourceFile_StreamIt
copy ..\GUI_Sample\Include\GUIConfig\Resources.dat ..\GUI_Sample\Include\GUIConfig\Resources.bin>nul 
echo.
pause
cd JLink
cls
"%BASE%\JLink.exe" -if JTAG -speed 12000 -device R7S721031 -jtagconf -1,-1 -CommanderScript FlashResources_StreamIt.Command
pause
cd ..
goto :loop

:errorNoResourceFile_StreamIt
echo.
echo Resource file not exists! Please export resources from GSE into "Export/StreamIt" folder in case of StreamIt!
echo.
pause
goto :loop

:errorInput
echo.
echo Illegal input! Please try again!
echo.
pause
goto :loop

