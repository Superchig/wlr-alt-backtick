set(CMAKE_SYSTEM_NAME "Linux" CACHE STRING "The operating system" FORCE)
set(CMAKE_SYSTEM_PROCESSOR "x86_64" CACHE STRING "Target CPU architecture" FORCE)
set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Target CPU architecture" FORCE)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
  set(SHELL_SCRIPT_EXT ".cmd")
else()
  set(SHELL_SCRIPT_EXT "")
endif()

# Use Zig as the compiler
set(CMAKE_C_COMPILER "zig-cc${SHELL_SCRIPT_EXT}" CACHE STRING "C compiler" FORCE)
set(CMAKE_CXX_COMPILER zig-c++${SHELL_SCRIPT_EXT} CACHE STRING "C++ compiler" FORCE)
set(CMAKE_ASM_COMPILER "zig-cc${SHELL_SCRIPT_EXT}" CACHE STRING "ASM compiler" FORCE)
if("${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" OR "${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
  set(CMAKE_C_COMPILER_TARGET "x86_64-linux-gnu.2.29")
  set(CMAKE_CXX_COMPILER_TARGET "x86_64-linux-gnu.2.29")
  set(CMAKE_ASM_COMPILER_TARGET "x86_64-linux-gnu.2.29")
endif()

# Configure the linker (Zig handles linking internally)
find_program(MOLD_PATH mold)

if(NOT "${MOLD_PATH}" STREQUAL "MOLD_PATH-NOTFOUND")
  set(CMAKE_LINKER_TYPE "MOLD" CACHE STRING "The C/C++ linker")
  set(CMAKE_LINKER mold CACHE STRING "Linker")
else()
  set(CMAKE_LINKER_TYPE "LLD" CACHE STRING "The C/C++ linker")
  set(CMAKE_LINKER "zig-ld.lld${SHELL_SCRIPT_EXT}" CACHE STRING "Linker")
  # set(CMAKE_C_USING_LINKER_LLD "-fuse-ld=zig-ld.lld")
  # set(CMAKE_CXX_USING_LINKER_LLD "-fuse-ld=zig-ld.lld")
endif()

# Use zig for ranlib
set(CMAKE_RANLIB "zig-ranlib${SHELL_SCRIPT_EXT}" CACHE STRING "Ranlib")

# Use zig for ar
set(CMAKE_AR "zig-ar${SHELL_SCRIPT_EXT}" CACHE STRING "AR")

# https://github.com/ziglang/zig/issues/22213
# Until `--dependency-file` is supported as a linker arg by `zig cc`, we need this
set(CMAKE_C_LINKER_DEPFILE_SUPPORTED FALSE)
set(CMAKE_CXX_LINKER_DEPFILE_SUPPORTED FALSE)

# Ensure CMake doesn’t search system paths for libs
set(CMAKE_FIND_ROOT_PATH "")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_C_COMPILER_EXTERNAL_TOOLCHAIN "")
set(CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN "")
