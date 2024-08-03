# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("${MODULE_NAME}")

include_directories(
        imgui
        xdl
        include)

set(IMGUI_SRC
      ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_android.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_opengl3.cpp)

aux_source_directory(xdl xdl-src)

set(DOBBY_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/libdobby.a)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
add_library(${CMAKE_PROJECT_NAME} SHARED
        # List C/C++ source files with relative paths to this CMakeLists.txt.
        main.cpp
        hook.cpp
		module.cpp
        ${IMGUI_SRC}
        ${xdl-src})

# Specifies libraries CMake should link to your target library. You
target_link_libraries(${CMAKE_PROJECT_NAME}
        # List libraries link to the target library
        android
        log
        EGL
        GLESv3
        ${DOBBY_LIB})