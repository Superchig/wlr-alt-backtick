cmake_minimum_required(VERSION 3.28)

project(wayland-scanner)

# Dependencies

set(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps" CACHE STRING "")
set(FETCHCONTENT_QUIET FALSE CACHE BOOL "")
set(DOWNLOAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/downloads")
include(FetchContent)

# LibXML
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(LIBXML2_WITH_CATALOG "Add the Catalog support" OFF)
option(LIBXML2_WITH_DEBUG "Add the debugging module" OFF)
option(LIBXML2_WITH_HTML "Add the HTML support" OFF)
option(LIBXML2_WITH_HTTP "Add the HTTP support" OFF)
option(LIBXML2_WITH_ICONV "Add ICONV support" OFF)
option(LIBXML2_WITH_ICU "Add ICU support" OFF)
option(LIBXML2_WITH_ISO8859X "Add ISO8859X support if no iconv" OFF)
option(LIBXML2_WITH_LEGACY "Add deprecated APIs for compatibility" OFF)
option(LIBXML2_WITH_LZMA "Use liblzma" OFF)
option(LIBXML2_WITH_MODULES "Add the dynamic modules support" OFF)
option(LIBXML2_WITH_OUTPUT "Add the serialization support" OFF)
option(LIBXML2_WITH_PATTERN "Add the xmlPattern selection interface" OFF)
option(LIBXML2_WITH_PROGRAMS "Build programs" OFF)
option(LIBXML2_WITH_PUSH "Add the PUSH parser interfaces" OFF)
option(LIBXML2_WITH_PYTHON "Build Python bindings" OFF)
option(LIBXML2_WITH_READLINE "readline support for xmllint shell" OFF)
option(LIBXML2_WITH_REGEXPS "Add Regular Expressions support" OFF)
option(LIBXML2_WITH_SAX1 "Add the older SAX1 interface" OFF)
option(LIBXML2_WITH_TESTS "Build tests" OFF)
option(LIBXML2_WITH_THREADS "Add multithread support" OFF)
option(LIBXML2_WITH_TLS "Enable thread-local storage" OFF)
option(LIBXML2_WITH_VALID "Add the DTD validation support" OFF)
option(LIBXML2_WITH_XINCLUDE "Add the XInclude support" OFF)
option(LIBXML2_WITH_XPATH "Add the XPATH support" ON)
FetchContent_Declare(
  libxml2
  URL https://gitlab.gnome.org/GNOME/libxml2/-/archive/v2.14.2/libxml2-v2.14.2.tar.gz
  URL_HASH SHA256=ed6632f1fca11e5243c9bf07100564b93c3890fecef626fc9d74562a473a85d6
  DOWNLOAD_DIR "${DOWNLOAD_DIR}"
)
FetchContent_MakeAvailable(libxml2)

# Libexpat
set(EXPAT_SHARED_LIBS OFF CACHE BOOLEAN "")
set(EXPAT_ENABLE_INSTALL OFF CACHE BOOLEAN "")
FetchContent_Declare(
  libexpat
  URL https://github.com/libexpat/libexpat/releases/download/R_2_7_1/expat-2.7.1.tar.gz
  URL_HASH SHA256=0cce2e6e69b327fc607b8ff264f4b66bdf71ead55a87ffd5f3143f535f15cfa2
  DOWNLOAD_DIR "${DOWNLOAD_DIR}"
)
FetchContent_MakeAvailable(libexpat)

# Libwayland

# TODO(Chris): Synchronize the libwayland version with the one in the main CMakeLists.txt
FetchContent_Declare(
  libwayland
  URL https://gitlab.freedesktop.org/wayland/wayland/-/archive/1.23.1/wayland-1.23.1.tar.bz2
  URL_HASH SHA256=4afcf2942a39d8276d06dcefc89dfaf029222994778fd4c49aa68a702ebf698f
  DOWNLOAD_DIR "${DOWNLOAD_DIR}"
)
FetchContent_MakeAvailable(libwayland)

# Libwayland - wayland-scanner
add_executable(
  wayland-scanner
  deps/libwayland-src/src/scanner.c
  deps/libwayland-src/src/wayland-util.c
)
target_include_directories(
  wayland-scanner
  PRIVATE
  ext/libwayland/wayland-scanner/include
)
target_link_libraries(wayland-scanner LibXml2)
target_link_libraries(wayland-scanner expat)
