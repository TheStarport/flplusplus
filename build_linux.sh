mkdir -p out
cd out
cmake "$@" -DCMAKE_TOOLCHAIN_FILE=../mingw-w64-i686.cmake ..
make && cp flplusplus/flplusplus.dll .
i686-w64-mingw32-strip flplusplus.dll
