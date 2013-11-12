#QtFabric uses Qt to wrap the fabricjs library cross platform

#Simple source for now
set(FABRIC_SRCS main.cpp)

#require Qt4 for this project
find_package(Qt4 REQUIRED)

#include our Box2D, IESoR, and JSON lib sources
include_directories (${Box2D_SOURCE_DIR} ${IESoR_SOURCE_DIR} ${JSON_SOURCE_DIR})

#include Qt Headers
include(${QT_USE_FILE})

#Set the executable as our main.cpp file for now
add_executable(QtFabric ${FABRIC_SRCS})

#don't forget to link in all our libraries
target_link_libraries(QtFabric ${QT_LIBRARIES} IESoR Box2D JSON)
