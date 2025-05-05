@echo off
:: ================================================================================
:: ================================================================================
:: - File:    static.bat
:: - Purpose: This file creates a static library version of c_string
::
:: Source Metadata
:: - Author:  Jonathan A. Webb
:: - Date:    January 11, 2025
:: - Version: 1.0
:: - Copyright: Copyright 2025, Jon Webb Inc.
:: ================================================================================
:: ================================================================================

:: Navigate to correct directory if needed
cd /d %~dp0\..\..\c_double

:: Create build directory if it doesn't exist
if not exist "build\static" mkdir build\static

:: Configure CMake
cmake -S . -B build\static -DBUILD_STATIC=ON

:: Build the project
cmake --build build\static

:: ================================================================================
:: ================================================================================
:: eof
