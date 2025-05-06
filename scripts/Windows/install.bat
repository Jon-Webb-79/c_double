@echo off
:: ================================================================================
:: ================================================================================
:: - File:    install.bat
:: - Purpose: Install/Update c_float and c_string library files
::
:: Source Metadata
:: - Author:  Jonathan A. Webb
:: - Date:    January 11, 2025
:: - Version: 1.1
:: - Copyright: Copyright 2025, Jon Webb Inc.
:: ================================================================================
:: ================================================================================

:: Check for administrator privileges
NET SESSION >nul 2>&1
if %errorLevel% neq 0 (
   echo Please run as Administrator
   pause
   exit /b 1
)

:: Set installation directories
set "FLOAT_INCLUDE_DIR=%ProgramFiles(x86)%\c_double\include"
set "FLOAT_LIB_DIR=%ProgramFiles(x86)%\c_double\lib"
set "STRING_INCLUDE_DIR=%ProgramFiles(x86)%\c_string\include"
set "STRING_LIB_DIR=%ProgramFiles(x86)%\c_string\lib"
set "BACKUP_DIR=%TEMP%\c_libs_backup"

:: Create directories if they don't exist
if not exist "%FLOAT_INCLUDE_DIR%" mkdir "%FLOAT_INCLUDE_DIR%"
if not exist "%FLOAT_LIB_DIR%" mkdir "%FLOAT_LIB_DIR%"
if not exist "%STRING_INCLUDE_DIR%" mkdir "%STRING_INCLUDE_DIR%"
if not exist "%STRING_LIB_DIR%" mkdir "%STRING_LIB_DIR%"
if not exist "%BACKUP_DIR%" mkdir "%BACKUP_DIR%"

:: Function to backup and install file
:install_file
if exist "%~2" (
   echo Backing up existing %~3...
   for /f "tokens=2-4 delims=/ " %%a in ('date /t') do (
       set "mydate=%%c%%a%%b"
   )
   for /f "tokens=1-2 delims=: " %%a in ('time /t') do (
       set "mytime=%%a%%b"
   )
   copy "%~2" "%BACKUP_DIR%\%~4_%mydate%_%mytime%.bak" >nul
   if errorlevel 1 (
       echo Error creating backup for %~3
       pause
       exit /b 1
   )
   echo Backup created successfully
) else (
   echo Installing new %~3...
)

copy /Y "%~1" "%~2"
if errorlevel 1 (
   echo Error installing %~3
   pause
   exit /b 1
)
echo %~3 installed/updated successfully
goto :eof

:: Install c_float files
echo Processing c_double library files...
call :install_file "..\..\c_double\c_float.h" "%FLOAT_INCLUDE_DIR%\c_double.h" "float header" "c_float.h"
call :install_file "..\..\c_double\c_float.c" "%FLOAT_LIB_DIR%\c_double.c" "float source" "c_float.c"

:: Install c_string files
echo.
echo Processing c_string library files...
call :install_file "..\..\c_double\c_string.h" "%STRING_INCLUDE_DIR%\c_string.h" "string header" "c_string.h"
call :install_file "..\..\c_double\c_string.c" "%STRING_LIB_DIR%\c_string.c" "string source" "c_string.c"

:: Update system environment variables
echo.
echo Updating system environment variables...
setx C_INCLUDE_PATH "%FLOAT_INCLUDE_DIR%;%STRING_INCLUDE_DIR%;%C_INCLUDE_PATH%" /M
setx LIBRARY_PATH "%FLOAT_LIB_DIR%;%STRING_LIB_DIR%;%LIBRARY_PATH%" /M

echo.
echo Installation/Update completed successfully
echo Backups (if any) are stored in: %BACKUP_DIR%
pause
:: ================================================================================
:: ================================================================================
:: eof
