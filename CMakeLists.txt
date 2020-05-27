cmake_minimum_required(VERSION 3.15)
project(aww CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

# sdl2
add_subdirectory(ext/SDL2)

# dear imgui
add_library(imgui
        ext/imgui/imgui.cpp
        ext/imgui/imgui_draw.cpp
        ext/imgui/imgui_widgets.cpp)
target_include_directories(imgui PUBLIC ext/imgui/)

# imnodes
add_library(imnodes
        ext/imnodes/imnodes.cpp)
target_include_directories(imnodes PUBLIC ext/imnodes/)
target_link_libraries(imnodes PUBLIC imgui)

# src
add_subdirectory(src)
