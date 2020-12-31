
add_library(ropemaker
  SHARED
    thread.c)

set_target_properties(ropemaker
  PROPERTIES
    C_VISIBILITY_PRESET hidden)

if (MSVC)
  # TODO
else()
  target_link_libraries(ropemaker
    PRIVATE
      pthread)
endif()
