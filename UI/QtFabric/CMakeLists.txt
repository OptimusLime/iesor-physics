# Smarter versions of moc wrapping macros that mimic qmake behavior
function(qt4_wrap_hdrs _moc_srcs)
  qt4_get_moc_flags(_moc_incs)
  set(_mocs)
  foreach(_current_file ${ARGN})
    get_filename_component(_abs_file ${_current_file} ABSOLUTE)
    if(EXISTS ${_abs_file})
      file(READ ${_abs_file} _contents)
      get_filename_component(_basename ${_abs_file} NAME_WE)
      string(REGEX MATCH "Q_OBJECT" _match "${_contents}")
      if(_match)
        set(_moc "${CMAKE_CURRENT_BINARY_DIR}/moc_${_basename}.cpp")
        qt4_create_moc_command(${_abs_file} ${_moc} "${_moc_incs}" "")
        macro_add_file_dependencies(${_abs_file} ${_moc})
        list(APPEND _mocs ${_moc})
      endif(_match)
    endif(EXISTS ${_abs_file})
  endforeach (_current_file)
  set(${_moc_srcs} ${_mocs} PARENT_SCOPE)
endfunction(qt4_wrap_hdrs)

function(qt4_wrap_srcs)
  qt4_get_moc_flags(_moc_incs)
  foreach(_current_file ${ARGN})
    get_filename_component(_abs_file ${_current_file} ABSOLUTE)
    if(EXISTS ${_abs_file})
      file(READ ${_abs_file} _contents)
      get_filename_component(_abs_path ${_abs_file} PATH)
      get_filename_component(_basename ${_abs_file} NAME_WE)
      string(REGEX MATCH "# *include +\"${_basename}\\.moc\"" _match "${_contents}")
      if(_match)
        string(REGEX MATCH "[^ <\"]+\\.moc" _moc "${_match}")
        set(_moc "${CMAKE_CURRENT_BINARY_DIR}/${_moc}")
        qt4_create_moc_command(${_abs_file} ${_moc} "${_moc_incs}" "")
        macro_add_file_dependencies(${_abs_file} ${_moc})
      endif(_match)
    endif(EXISTS ${_abs_file})
  endforeach(_current_file)
endfunction(qt4_wrap_srcs)

#QtFabric uses Qt to wrap the fabricjs library cross platform

#require Qt4 for this project, need webkit too!
find_package(Qt4 COMPONENTS QtCore QtGui QtWebKit REQUIRED)
SET(QT_USE_QTWEBKIT TRUE) 

#Executable file
set(FABRIC_MAIN main.cpp)

#Simple source for now
set(FABRIC_SRCS 
	bridge/JSBridge.cpp
	view/qtFabric.cpp
)

set(FABRIC_HDRS
	bridge/JSBridge.h
	view/qtFabric.h
)

SET(FABRIC_RESOURCES Resources/qtfabric.qrc)

#wrap up required objects
qt4_wrap_cpp(FABRIC_HDRS_MOC ${FABRIC_HDRS})
#No forms to wrap
#QT4_WRAP_UI(FABRIC_FORMS_HEADERS ${FABRIC_FORMS})
qt4_add_resources(FABRIC_RESOURCES_RCC ${FABRIC_RESOURCES})

#qt4_wrap_hdrs(_moc_srcs ${_hdrs})
#qt4_wrap_srcs(${_srcs})
#qt4_wrap_ui(_forms_hdrs ${_forms})
#qt4_add_resources(_rsrcs_srcs ${_rsrcs})
#add_library(${_name} ${_srcs} ${_moc_srcs} ${_rsrcs_srcs})





#include Qt Headers
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

#include our Box2D, IESoR, and JSON lib sources
include_directories (${BOX2D_SOURCE_DIR} ${IESOR_SOURCE_DIR} ${JSON_SOURCE_DIR})

#include our Physics headers
include_directories(../../Physics)
include_directories( ../../UI )
include_directories(${CMAKE_CURRENT_BINARY_DIR})


#visual studio settings
source_group(Src FILES ${FABRIC_SRCS} ${FABRIC_MAIN})
source_group(Include FILES ${FABRIC_HDRS})

#Set the executable using our wrapped headers
add_executable(QtFabric ${FABRIC_MAIN} ${FABRIC_SRCS} ${FABRIC_HDRS} ${FABRIC_HDRS_MOC} ${FABRIC_RESOURCES_RCC})

#don't forget to link in all our libraries
#${QT_QTWEBKIT_LIBRARY} 
target_link_libraries(QtFabric ${QT_LIBRARIES} IESoR Box2D JSON)
