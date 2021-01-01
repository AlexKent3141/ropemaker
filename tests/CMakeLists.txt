
add_executable(test1
  test1.c)

target_link_libraries(test1
  PRIVATE
    ropemaker)

add_executable(test2
  test2.c)

target_link_libraries(test2
  PRIVATE
    ropemaker)
