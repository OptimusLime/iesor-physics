cmake_minimum_required(VERSION 2.6)

#cross-platform iesor project, including UI
project(IESoRx)


option(BOX2D_BUILD_STATIC "Build Box2D static libraries" ON)
option(IESOR_BUILD_STATIC "Build IESoR static libraries" ON)
option(JSON_BUILD_STATIC "Build JSON static libraries" ON)
option(QT_BUILD_STATIC "Build QtFabric static libraries" ON)

#set(BOX2D_BUILD_STATIC ON)
#set(IESOR_BUILD_STATIC ON)
#set(JSON_BUILD_STATIC ON)
#set(QT_BUILD_STATIC ON)

set(BOX2D_VERSION 2.3.0)
set(IESOR_VERSION 0.0.1)
set(JSON_VERSION 0.5.0)
set(IESORX_VERSION 0.0.1)

# Add our Box2d Library -- back a directory
add_subdirectory(Physics/Box2D)

# Add our IESoR Library
add_subdirectory(Physics/IESoR)

#add json support library
add_subdirectory(Physics/JSON)

# Qt wrapping Fabricjs project.
add_subdirectory(UI/QtFabric)
