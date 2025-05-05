@echo off
:: ================================================================================
:: ================================================================================
:: - File:    debug.bat
:: - Purpose: This file contains a script that will build C software
::            using CMake for Windows environments
::
:: Source Metadata
:: - Author:  Jonathan A. Webb
:: - Date:    February 26, 2022
:: - Version: 1.0
:: - Copyright: Copyright 2022, Jon Webb Inc.
:: ================================================================================
:: ================================================================================

:: Navigate to correct directory if needed (adjust paths as necessary)
cd /d %~dp0\..\..\c_double

:: Create build directory if it doesn't exist
if not exist "build\debug" mkdir build\debug

:: Configure CMake
cmake -S . -B build\debug -DCMAKE_BUILD_TYPE=Debug

:: Build the project
cmake --build build\debug --config Debug

:: ================================================================================
:: ================================================================================
:: eof
