cmake_minimum_required(VERSION 3.1.3)
project(PixelToaster VERSION 1.5.0)

option (PIXELTOASTER_TINY   "Disable use of STL and CRT libraries." NO)
option (PIXELTOASTER_NO_STL "Disable use of STL library." NO)
option (PIXELTOASTER_NO_CRT "Disable use of CRT library." NO)
if (MSVC)
    option (USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC runtime library DLL" YES)
endif()
#ifdef PIXELTOASTER_TINY
#    define PIXELTOASTER_NO_STL
#    define PIXELTOASTER_NO_CRT
#option (PIXELTOASTER_ENABLE_UNIT_TESTS "Enable unit tests." ON)
option (ENABLE_EXAMPLES   "Enable examples." YES)
#option (PIXELTOASTER_ENABLE_INSTALL    "Enable install." ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    if (NOT USE_MSVC_RUNTIME_LIBRARY_DLL)
        foreach (flag
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL
            CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL
            CMAKE_CXX_FLAGS_RELWITHDEBINFO
        )

            if (${flag} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
            endif()
            if (${flag} MATCHES "/MDd")
                string(REGEX REPLACE "/MDd" "/MTd" ${flag} "${${flag}}")
            endif()
        endforeach()
    endif()
endif()

#include(CTest)
#enable_testing()

add_library(PixelToaster
    PixelToaster.cpp
    PixelToaster.h
    PixelToasterCommon.h
    PixelToasterConversion.h

    PixelToasterApple.h
    PixelToasterUnix.h
    PixelToasterWindows.h
)

if (PIXELTOASTER_TINY)
    target_compile_definitions(PixelToaster PUBLIC PIXELTOASTER_TINY)
endif()

if (PIXELTOASTER_NO_STL AND (NOT PIXELTOASTER_TINY))
    target_compile_definitions(PixelToaster PUBLIC PIXELTOASTER_NO_STL)
endif()

if (PIXELTOASTER_NO_CRT AND (NOT PIXELTOASTER_TINY))
    target_compile_definitions(PixelToaster PUBLIC PIXELTOASTER_NO_CRT)
endif()

if (BUILD_SHARED_LIBS)
    target_compile_definitions(PixelToaster PUBLIC PIXELTOASTER_DYNAMIC)
    target_compile_definitions(PixelToaster PRIVATE PIXELTOASTER_DLL)
endif()

if (APPLE)
    target_sources(PixelToaster PRIVATE
        PixelToasterApple.mm
    )
    target_link_libraries(PixelToaster PRIVATE
        "-framework CoreServices"
        "-framework Cocoa"
        "-framework OpenGL"
    )
elseif (UNIX)
    target_link_libraries(PixelToaster PRIVATE
        X11
        rt
    )
endif()

if (ENABLE_EXAMPLES)
    macro(add_exapmle target)
        add_executable(${target} ${ARGN})
        target_link_libraries(${target} PRIVATE PixelToaster)
        if (WIN32)
            target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS)
        endif()
    endmacro()

    add_exapmle(ExampleFloatingPoint    ExampleFloatingPoint.cpp)
    add_exapmle(ExampleFullscreen       ExampleFullscreen.cpp)
    add_exapmle(ExampleImage            ExampleImage.cpp)
    add_exapmle(ExampleKeyboardAndMouse ExampleKeyboardAndMouse.cpp)
    add_exapmle(ExampleMultiDisplay     ExampleMultiDisplay.cpp)
    add_exapmle(ExampleTimer            ExampleTimer.cpp)
    add_exapmle(ExampleTrueColor        ExampleTrueColor.cpp)
endif()

#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)
