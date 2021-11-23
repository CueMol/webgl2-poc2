include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}/src
  ${Boost_INCLUDE_DIRS}
  )

# Normal C++ source files
SET(EMBR_SRCS
  init_cuemol.cpp
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

set_target_properties(embr PROPERTIES LINK_FLAGS "--bind -s USE_BOOST_HEADERS=1 -lnodefs.js")
