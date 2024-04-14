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
    "src/codec.cpp"
    "src/adoxa/adoxa.cpp"
    "src/adoxa/wheelscroll.cpp"
    "src/startlocation.cpp"
    "src/fontresource.cpp"
    "src/Common.cpp"
    "src/thnplayer.cpp"
    "src/consolewindow.cpp"
)

if (MSVC)
    set(COMMON_DEF_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/def/Common.def")
    set(COMMON_LIB_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/Common.lib")

    add_custom_command(
            OUTPUT ${COMMON_LIB_LOCATION}
            COMMAND lib /def:${COMMON_DEF_LOCATION} /MACHINE:X86 /out:${COMMON_LIB_LOCATION}
            DEPENDS ${COMMON_DEF_LOCATION}
    )

    add_custom_target(common_lib
            DEPENDS ${COMMON_LIB_LOCATION})

    add_dependencies(flplusplus common_lib)

    target_link_libraries(flplusplus ${COMMON_LIB_LOCATION})
endif()

target_link_libraries(flplusplus shlwapi gdiplus shell32 dbghelp)
target_include_directories(flplusplus PUBLIC "include")
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(flplusplus PUBLIC -static-libgcc -static-libstdc++ -static)
endif()
set_target_properties(flplusplus PROPERTIES PREFIX "")
