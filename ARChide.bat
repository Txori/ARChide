@echo off
setlocal enabledelayedexpansion

rem Set the working directory to the folder containing the script
cd /d "%~dp0"

rem Set the input file name
set "inputFile=games.csv"
set "configFile=conf.txt"
set "deleteFolder=_hidden"

rem Check if games list exist
if not exist "%inputFile%" (
    echo Error: %inputFile% not found. Please make sure the file exists.
    pause
    exit /b 1
)

rem Check if config files exist
if not exist "%configFile%" (
    echo Error: %configFile% not found. Please make sure the file exists.
    pause
    exit /b 1
)

rem Create the "delete" folder if it doesn't exist
if not exist "%deleteFolder%" mkdir "%deleteFolder%"

rem Read the configuration file into an array
for /f "tokens=1,* delims==" %%a in (%configFile%) do (
    set "system[%%a]=%%b"
)

rem First pass: Check if all systems of hidden games are listed in the config file
for /f "tokens=1-8 delims=," %%a in (%inputFile%) do (
    rem Check if the line contains "HIDDEN" in the "Hidden" column %%g
    if "%%g"=="HIDDEN" (
        rem Get the folder name for the system from the array
        if defined system[%%a] (
            set "folder=!system[%%a]!"
        ) else (
            echo Missing system in config file: %%a
            echo Please add it and run the script again.
            pause
            exit /b 1
        )
    )
)

rem Second pass: Loop through each line in the input file and move files
for /f "tokens=1-8 delims=," %%a in (%inputFile%) do (
    rem Check if the line contains "HIDDEN" in the "Hidden" column
    if "%%g"=="HIDDEN" (
        rem Get the folder name for the system from the array
        if defined system[%%a] (
            set "folder=!system[%%a]!"
            rem Build the full path to the file
            set "filePath=!folder!\%%b"
            rem Remove double quotes from the file path
            set "filePath=!filePath:"=!"
            rem Check if the file exists
            if exist "!filePath!" (
                rem Create the system folder in the "delete" folder if it doesn't exist
                if not exist "%deleteFolder%\!folder!" mkdir "%deleteFolder%\!folder!"
                
                rem Move the file to the "delete" folder and echo the result
                move "!filePath!" "%deleteFolder%\!filePath!" >nul && echo Moved: !filePath! || echo Failed to move: !filePath!
            ) else (
                echo File not found: !filePath!
            )
        ) else (
            echo System not found in config file: %%a
        )
    )
)

echo .
echo Hidden games moved to %deleteFolder% folder. Check the results above.
pause


endlocal
