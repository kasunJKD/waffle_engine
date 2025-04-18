add_library(libc_w STATIC
  w_math.cpp
)

target_include_directories(libc_w PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
