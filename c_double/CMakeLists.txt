# ================================================================================
# ================================================================================
# - File:    CMakeLists.txt
# - Purpose: Primary CMake file for C build system
#
# Source Metadata
# - Author:  Jonathan A. Webb
# - Date:    December 30, 2024
# - Version: 1.0
# - Copyright: Copyright 2024, Jonathan A. Webb Inc.
# ================================================================================
# ================================================================================

cmake_minimum_required(VERSION 3.31.3)
project(c_double)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Option for static build
option(BUILD_STATIC "Build static library" OFF)

# Set compiler flags based on compiler type
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wpedantic -O3 -march=native")
# if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wpedantic -O3, -march=native")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX")
    add_definitions(
        /wd4996
        /D_CRT_SECURE_NO_WARNINGS
    )
endif()

# Platform-specific configurations
if(UNIX AND NOT APPLE)
    add_definitions(-D_GNU_SOURCE)
elseif(APPLE)
    add_definitions(-D_DARWIN_C_SOURCE)
elseif(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

# Add the library
if(BUILD_STATIC)
    add_library(c_double STATIC
        c_double.c
        c_string.c
    )
    
    target_include_directories(c_float 
        PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/c_double/include
    )
    
    # Link with math library
    target_link_libraries(c_double PUBLIC m)
    
    # Set output directory for static library
    if(WIN32)
        set_target_properties(c_double
            PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )
    else()
        set_target_properties(c_double
            PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )
    endif()
else()
    # Debug build with tests
    enable_testing()
    add_library(c_double
        c_double.c
        c_string.c
    )
    
    target_include_directories(c_double
        PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/c_double/include
    )

    # Link with math library
    target_link_libraries(c_double PUBLIC m)
    
    if(WIN32)
        set_target_properties(c_float
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug"
            LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug"
        )
    endif()
    
    # Add the test directory only for debug build
    add_subdirectory(test)
endif()
# ================================================================================
# ================================================================================
# eof
