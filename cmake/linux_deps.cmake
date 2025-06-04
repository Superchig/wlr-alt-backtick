# Libffi (transitive dependency of libwayland)
FetchContent_Declare(
  libffi
  URL https://github.com/libffi/libffi/releases/download/v3.4.8/libffi-3.4.8.tar.gz
  URL_HASH SHA256=bc9842a18898bfacb0ed1252c4febcc7e78fa139fd27fdc7a3e30d9d9356119b
)
FetchContent_MakeAvailable(libffi)
add_library(
  libffi
  STATIC
  deps/libffi-src/src/closures.c
  deps/libffi-src/src/types.c
  deps/libffi-src/src/x86/ffi64.c
  deps/libffi-src/src/x86/ffiw64.c
  deps/libffi-src/src/x86/unix64.S
  deps/libffi-src/src/x86/win64.S
  deps/libffi-src/src/tramp.c
  deps/libffi-src/src/prep_cif.c
)
target_include_directories(
  libffi
  SYSTEM
  PUBLIC
  cmake/libffi/ext/include # For ffi.h and fficonfig.h
  deps/libffi-src/src/x86 # For ffitarget.h
  deps/libffi-src/include # For ffi_common.h
)

# Libwayland

FetchContent_Declare(
  libwayland
  URL https://gitlab.freedesktop.org/wayland/wayland/-/archive/1.23.1/wayland-1.23.1.tar.bz2
  URL_HASH SHA256=4afcf2942a39d8276d06dcefc89dfaf029222994778fd4c49aa68a702ebf698f
)
FetchContent_MakeAvailable(libwayland)

# Wayland-scanner (which technically uses its own copy of libwayland)

set(WAYLAND_SCANNER "${CMAKE_CURRENT_SOURCE_DIR}/cmake/wayland-scanner")
set(WAYLAND_SCANNER_EXE "${WAYLAND_SCANNER}/build/wayland-scanner") # TODO(Chris): Use .exe in Windows
set(LIBWAYLAND_SRC "${FETCHCONTENT_BASE_DIR}/libwayland-src")

add_custom_command(
  OUTPUT "${WAYLAND_SCANNER_EXE}"
  COMMAND just build # So that we use the host's native C compiler and produce a native exectuable
  WORKING_DIRECTORY "${WAYLAND_SCANNER}"
)

add_custom_command(
  OUTPUT "${LIBWAYLAND_SRC}/src/wayland-client-protocol.h" # Necessary for wayland-client to build properly
  COMMAND "${WAYLAND_SCANNER_EXE}" client-header protocol/wayland.xml src/wayland-client-protocol.h
  WORKING_DIRECTORY "${LIBWAYLAND_SRC}"
  DEPENDS "${WAYLAND_SCANNER_EXE}" "${LIBWAYLAND_SRC}/protocol/wayland.xml"
)

add_custom_command(
  OUTPUT "${LIBWAYLAND_SRC}/src/wayland-client-protocol.c" # Necessary for wayland-client to build properly
  COMMAND "${WAYLAND_SCANNER_EXE}" public-code protocol/wayland.xml src/wayland-client-protocol.c
  WORKING_DIRECTORY "${LIBWAYLAND_SRC}"
  DEPENDS "${WAYLAND_SCANNER_EXE}" "${LIBWAYLAND_SRC}/protocol/wayland.xml"
)

function(ADD_WAYLAND_COMMAND PROTOCOL)
  set(PROTOCOL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vendor/wayland-protocols/${PROTOCOL}.xml")

  add_custom_command(
    OUTPUT "${LIBWAYLAND_SRC}/src/${PROTOCOL}.c"
    COMMAND "${WAYLAND_SCANNER_EXE}" public-code "${PROTOCOL_PATH}" "src/${PROTOCOL}.c"
    WORKING_DIRECTORY "${LIBWAYLAND_SRC}"
    DEPENDS "${WAYLAND_SCANNER_EXE}" "${PROTOCOL_PATH}"
  )

  add_custom_command(
    OUTPUT "${LIBWAYLAND_SRC}/src/${PROTOCOL}.h"
    COMMAND "${WAYLAND_SCANNER_EXE}" client-header "${PROTOCOL_PATH}" "src/${PROTOCOL}.h"
    WORKING_DIRECTORY "${LIBWAYLAND_SRC}"
    DEPENDS "${WAYLAND_SCANNER_EXE}" "${PROTOCOL_PATH}"
  )
endfunction()

add_wayland_command(ext-foreign-toplevel-list-v1)
add_wayland_command(wlr-foreign-toplevel-management-unstable-v1)

# Libwayland - wayland-client

add_library(
  wayland-client
  SHARED
  ${LIBWAYLAND_SRC}/src/connection.c
  ${LIBWAYLAND_SRC}/src/wayland-client.c
  ${LIBWAYLAND_SRC}/src/wayland-os.c
  ${LIBWAYLAND_SRC}/src/wayland-util.c

  "${LIBWAYLAND_SRC}/src/wayland-client-protocol.c"
  "${LIBWAYLAND_SRC}/src/wayland-client-protocol.h"
  "${LIBWAYLAND_SRC}/src/wlr-foreign-toplevel-management-unstable-v1.c"
  "${LIBWAYLAND_SRC}/src/wlr-foreign-toplevel-management-unstable-v1.h"
)
link_lib(wayland-client)
target_include_directories(wayland-client SYSTEM PUBLIC cmake/wayland-scanner/ext/libwayland/wayland-scanner/include) # For wayland-version.h
target_include_directories(wayland-client SYSTEM PUBLIC cmake/wayland-client/ext/include) # For ../config.h
target_include_directories(wayland-client SYSTEM INTERFACE deps/libwayland-src/src) # For <wayland-client.h>
target_link_libraries(wayland-client PUBLIC libffi)
