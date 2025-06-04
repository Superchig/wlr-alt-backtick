run: build
    build-linux/wlr-alt-backtick

build:
    cmake . -B build-linux -G Ninja
    ninja -C build-linux

build-linux-release:
    #!/bin/sh
    export PATH="$PATH:$PWD/scripts"
    cmake . -B build-linux-release -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-linux-zig.cmake
    ninja -C build-linux-release

clean:
    rm -rf build-* dist-*

deep-clean: clean
    find deps -mindepth 1 -maxdepth 1 | grep -vE '\-tmp' | xargs -I {} rm -rf {}
