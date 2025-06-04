cmake_minimum_required(VERSION 3.30)

# Set up project basics
project(wlr-alt-backtick)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

set(CMAKE_POSITION_INDEPENDENT_CODE true)
set(CMAKE_CXX_SCAN_FOR_MODULES false) # Until Zig c++ supports clang-scan-deps better, we won't even try to use modules

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  enable_language(ASM)
endif()

# Set up ccache
# find_program(CCACHE_PROGRAM ccache)
# if(CCACHE_PROGRAM)
#   message(STATUS "Found ccache in ${CCACHE_PROGRAM}")
#   set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
# endif()

add_executable(wlr-alt-backtick src/main.cpp)

# Dependencies

function(LINK_LIB LIB)
  target_link_libraries(wlr-alt-backtick "${LIB}")
endfunction()

cmake_policy(SET CMP0168 NEW) # Don't use a separate sub-build for FetchContent
set(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps" CACHE STRING "")
include(FetchContent)

include("cmake/linux_deps.cmake")
