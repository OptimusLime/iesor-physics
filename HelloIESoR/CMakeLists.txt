# HelloIESoR Example
include_directories (${Box2D_SOURCE_DIR} ${IESoR_SOURCE_DIR})
add_executable(HelloIESoR HelloIESoR.cpp)
target_link_libraries (HelloIESoR IESoR Box2D)
