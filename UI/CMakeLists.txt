cmake_minimum_required(VERSION 2.6)

#cross-platform iesor project, including UI
project(IESoRx)

set(BOX2D_BUILD_STATIC 1.0)
set(IESOR_BUILD_STATIC 1.0)
set(JSON_BUILD_STATIC 1.0)
set(QT_BUILD_STATIC 1.0)

set(IESORX_VERSION 0.0.1)

# Qt wrapping Fabricjs project.
add_subdirectory(QtFabric)

# Add our Box2d Library -- back a directory
add_subdirectory(../Physics/Box2D)

# Add our IESoR Library
add_subdirectory(../Physics/IESoR)

#add json support library
add_subdirectory(../Physics/JSON)
