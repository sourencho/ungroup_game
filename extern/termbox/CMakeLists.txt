#################################################################
#   Termbox - A ncurses alternative
#
#   Author: nsf - no.smile.face@gmail.com
#   Website: http://code.google.com/p/termbox/
#
#   Termbox library is a simple and clean ncurses alternative.
#   Of course everything has it's own price.
#   In this case it's portability and stability.
#
#   Modified by: ykj - youngkevinjonas@gmail.com
#   On: May 5th 2012
#   I have completely reorganized the project
#   while leaving most of the code intact (fixed
#   a couple of warnings). Now you can either use
#   it as a dependency for a C/C++ project by employing
#   CMake, or you can install it as a Python package
#   by using Python distutils and Cython.
#
#   This file constitutes the CMake build.
#################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
PROJECT(termbox)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=gnu99")

# Used the jsoncpp CMakeLists.txt as inspiration (read: copied most of it)
OPTION(TERMBOX_WITH_WARNING_AS_ERROR "Force compilation to fail if a warning occurs" OFF)

# Ensures that CMAKE_BUILD_TYPE is visible in cmake-gui on Unix
IF(NOT WIN32)
    IF(NOT CMAKE_BUILD_TYPE)
        SET(CMAKE_BUILD_TYPE Release CACHE STRING
            "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Coverage."
            FORCE)
    ENDIF(NOT CMAKE_BUILD_TYPE)
ENDIF(NOT WIN32)

SET(LIB_SUFFIX "" CACHE STRING "Optional arch-dependent suffix for the library installation directory")

SET(RUNTIME_INSTALL_DIR bin
    CACHE PATH "Install dir for executables and dlls")
SET(ARCHIVE_INSTALL_DIR lib${LIB_SUFFIX}
    CACHE PATH "Install dir for static libraries")
SET(LIBRARY_INSTALL_DIR lib${LIB_SUFFIX}
    CACHE PATH "Install dir for shared libraries")
SET(INCLUDE_INSTALL_DIR include
    CACHE PATH "Install dir for headers")
SET(PACKAGE_INSTALL_DIR lib${LIB_SUFFIX}/cmake
    CACHE PATH "Install dir for cmake package config files")
MARK_AS_ADVANCED( RUNTIME_INSTALL_DIR ARCHIVE_INSTALL_DIR INCLUDE_INSTALL_DIR PACKAGE_INSTALL_DIR )

# Set variable named ${VAR_NAME} to value ${VALUE}
FUNCTION(set_using_dynamic_name VAR_NAME VALUE)
    SET( "${VAR_NAME}" "${VALUE}" PARENT_SCOPE)
ENDFUNCTION(set_using_dynamic_name)

macro(UseCompilationWarningAsError)
    if ( MSVC )
        # Only enabled in debug because some old versions of VS STL generate
        # warnings when compiled in release configuration.
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /WX ")
    endif( MSVC )
endmacro()

# Include our configuration header
INCLUDE_DIRECTORIES( ${termbox_SOURCE_DIR}/include )

IF(TERMBOX_WITH_WARNING_AS_ERROR)
    UseCompilationWarningAsError()
ENDIF(TERMBOX_WITH_WARNING_AS_ERROR)

# Build the different applications
ADD_SUBDIRECTORY( src )
