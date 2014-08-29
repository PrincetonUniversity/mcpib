cmake_minimum_required(VERSION 2.8)
project(mcpib)

enable_testing()

add_subdirectory(cmake)

include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${PYTHON_INCLUDE_DIRS}
)

add_subdirectory(lib)
add_subdirectory(tests)