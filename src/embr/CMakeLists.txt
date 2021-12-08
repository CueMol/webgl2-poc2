include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}/src
  ${Boost_INCLUDE_DIRS}
  )

# Normal C++ source files
SET(EMBR_SRCS
  init_cuemol.cpp
  # wrapper.cpp
  direct.cpp
)

SET(EMBR_LINK_LIBRARIES
  pcre
  expat
  # qmpng
  # qmzlib
  qlib gfx qsys
  )

# SET(EMCC_PRE_JS "--pre-js ${CMAKE_CURRENT_SOURCE_DIR}/pre.js")
SET(EMCC_PRE_JS "--preload-file ${CMAKE_SOURCE_DIR}/src/data@ --pre-js ${CMAKE_CURRENT_SOURCE_DIR}/pre.js")

add_executable(embr ${EMBR_SRCS})
target_link_libraries(embr PRIVATE ${EMBR_LINK_LIBRARIES})

set_target_properties(embr PROPERTIES LINK_FLAGS "--no-entry -s EXPORT_ES6=1 -s MODULARIZE=1 -s USE_BOOST_HEADERS=1 -fexceptions ${EMCC_PRE_JS} -s EXPORTED_RUNTIME_METHODS=intArrayFromString,allocate,ALLOC_NORMAL,UTF8ToString")
#
#

SET(JS_SCRIPTS
  utils.js
  utils_module.js
  )

add_custom_command(
  TARGET embr POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/utils_module.js
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libs/utils_module.js
  )
