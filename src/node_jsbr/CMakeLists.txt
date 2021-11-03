# Include node-addon-api wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
message(STATUS "NODE_ADDON_API_DIR: ${NODE_ADDON_API_DIR}")

# Essential include files to build a node addon,
# you should add this line in every CMake.js based project.

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}/src
  ${Boost_INCLUDE_DIRS}
  # ${Python3_INCLUDE_DIRS}
  ${NODE_ADDON_API_DIR}
  ${CMAKE_JS_INC}
  )


# Normal C++ source files
SET(NODE_JSBR_SRCS
  init_cuemol.cpp
  wrapper.cpp
  )

if (WIN32)
  add_definitions(-DNODE_JSBR_EXPORTS=1)
endif()

SET(NODE_JSBR_LINK_LIBRARIES
  pcre
  # expat
  # qmpng
  # qmzlib
  qlib
  Boost::filesystem
  ${CMAKE_JS_LIB}
  )

add_library(node_jsbr SHARED ${NODE_JSBR_SRCS})
target_link_libraries(node_jsbr PRIVATE ${NODE_JSBR_LINK_LIBRARIES})

# This line will give our library file a .node extension without any "lib" prefix
set_target_properties(node_jsbr PROPERTIES PREFIX "" SUFFIX ".node")

install(TARGETS node_jsbr
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )
