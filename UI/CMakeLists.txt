cmake_minimum_required(VERSION 2.6)

#cross-platform iesor project, including UI
project(IESoRx)

#option(BOX2D_BUILD_STATIC "Build Box2D static libraries" ON)
#option(IESOR_BUILD_STATIC "Build IESoR static libraries" ON)
#option(JSON_BUILD_STATIC "Build JSON static libraries" ON)
#option(QT_BUILD_STATIC "Build QtFabric static libraries" ON)

OPTION(OSG_BUILD_PLATFORM_IPHONE "Enable IPhoneSDK Device support" ON)
OPTION(OSG_BUILD_PLATFORM_IPHONE_SIMULATOR "Enable IPhoneSDK Simulator support" ON) 

set(CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS7.0.sdk)

set(BOX2D_BUILD_STATIC ON)
set(IESOR_BUILD_STATIC ON)
set(JSON_BUILD_STATIC ON)
#set(QT_BUILD_STATIC ON)

set(BOX2D_VERSION 2.3.0)
set(IESOR_VERSION 0.0.1)
set(JSON_VERSION 0.5.0)
set(IESORX_VERSION 0.0.1)

#set(BOX2D_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Physics/Box2D")
#set(IESOR_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Physics/IESoR")
#set(JSON_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Physics/JSON")

# Add our Box2d Library -- back a directory
add_subdirectory(../Physics/Box2D "${CMAKE_CURRENT_BINARY_DIR}/Box2D")

# Add our IESoR Library
add_subdirectory(../Physics/IESoR "${CMAKE_CURRENT_BINARY_DIR}/IESoR")

#add json support library
add_subdirectory(../Physics/JSON "${CMAKE_CURRENT_BINARY_DIR}/JSON")

# Qt wrapping Fabricjs project.
add_subdirectory(QtFabric)