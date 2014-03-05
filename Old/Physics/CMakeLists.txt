cmake_minimum_required(VERSION 2.6)

project(IESoR)

option(BOX2D_BUILD_SHARED "Build Box2D shared libraries" OFF)
option(BOX2D_BUILD_STATIC "Build Box2D static libraries" ON)

option(IESOR_BUILD_SHARED "Build IESoR shared libraries" OFF)
option(IESOR_BUILD_STATIC "Build IESoR static libraries" ON)

option(JSON_BUILD_SHARED "Build JSON shared libraries" OFF)
option(JSON_BUILD_STATIC "Build JSON static libraries" ON)

set(BOX2D_VERSION 2.3.0)
set(IESOR_VERSION 0.0.1)
set(JSON_VERSION 0.5.0)

#set(LIB_INSTALL_DIR lib${LIB_SUFFIX})

# The Box2D library.
add_subdirectory(Box2D)

# Add our IESoR Library
add_subdirectory(IESoR)

#add json support library
add_subdirectory(JSON)

# Add our IESoR Executable
add_subdirectory(HelloIESoR)
