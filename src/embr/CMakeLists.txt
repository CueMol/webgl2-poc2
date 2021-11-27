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

add_executable(embr ${EMBR_SRCS})
target_link_libraries(embr PRIVATE ${EMBR_LINK_LIBRARIES})

set_target_properties(embr PROPERTIES LINK_FLAGS "-s USE_BOOST_HEADERS=1 -fexceptions -lnodefs.js -s EXPORTED_RUNTIME_METHODS=intArrayFromString,allocate,ALLOC_NORMAL,UTF8ToString")

SET(JS_SCRIPTS
  utils.js
  )

add_custom_command(
  TARGET embr POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/utils.js
  ${CMAKE_BINARY_DIR}/javascript/utils.js
  )
