cmake_minimum_required(VERSION 2.8)
project(mcpib)

enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb")

add_subdirectory(cmake)

include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${PYTHON_INCLUDE_DIRS}
)

add_subdirectory(lib)
add_subdirectory(python/mcpib)
add_subdirectory(tests)

