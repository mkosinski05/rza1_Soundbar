    @echo off
    echo LOAD BOOT LOADER INTO Single QSPI FLASH 
    echo ===================================================================
    echo ***    Remember to update the JLink.exe path specified          ***
    echo ***    in this batch file if using a different version than     ***
    echo ***    that installed by the RSK+RZA1H_V2 installer.            ***
    echo ***    This batch file was tested and works with Ver 6.12f      ***
    echo ***    of the JLink.exe                                         ***
    echo ***                                                             ***
    echo ***    Please refer to the RSK+RZA1H Tutorial Manual            *** 
    echo ***    for more information on generating the binary files.     ***
    echo ===================================================================
    echo ------------------------------------------------------------------------
    setlocal enableextensions 

	set programmer2=JLink.exe
    set desiredversion=0xbf6d

    for /f "tokens=2*" %%A in ('REG QUERY "hkcu\Software\SEGGER\J-Link" /v CurrentVersion') DO (
      for %%F in (%%B) do (
		set imagepath=%%F 
		set version=%%B
		goto :Version_set
      )
    )
    echo Please Install J-link onto your host PC.
    echo download the latest version of J-Link from 
    echo https://www.segger.com/downloads/jlink
    pause
    goto END
    
:Version_set
    IF NOT %version% GEQ %desiredversion% echo J-link May be incompatible version required is %desiredversion% version found is %version% 
    IF NOT %version% GEQ %desiredversion% PAUSE
	
    for /f "tokens=2*" %%A in ('REG QUERY "hkcu\Software\SEGGER\J-Link" /v InstallPath') DO (
      for %%F in (%%B) do (
		set imagepath=%%F 
		set programmer=%%B
		goto :Programmer_set
      )
    )
    goto END
		
:Programmer_set
    "%programmer%%programmer2%" -speed 12000 -if JTAG -JTAGConf -1,-1 -device R7S721031_DUALSPI -CommanderScript program_qspi_loader_application.Command
    echo ------------------------------------------------------------------------
    echo That's it.
:END
    set version=
    set imagepath=
    set programmer=
    set programmer2=
    set desiredversion=
 