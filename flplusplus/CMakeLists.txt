# CMakeList.txt : CMake project for flplusplus, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.13)

# Add source to this project's executable.
add_library (flplusplus SHARED 
    "src/flplusplus.cpp" 
    "src/patch.cpp"
    "src/graphics.cpp"
    "src/config.cpp"
    "src/screenshot.cpp"
    "src/savegame.cpp"
    "src/log.cpp"
    "src/startlocation.cpp"
    "src/fontresource.cpp"
    "src/thnplayer.cpp"
    "src/consolewindow.cpp"
    "src/Common.cpp"
    "src/shippreviewscroll.cpp"
)

target_link_libraries(flplusplus shlwapi gdiplus shell32 dbghelp)
target_include_directories(flplusplus PUBLIC "include")
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(flplusplus PUBLIC -static-libgcc -static-libstdc++ -static)
endif()
set_target_properties(flplusplus PROPERTIES PREFIX "")
