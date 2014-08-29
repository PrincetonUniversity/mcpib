
python_add_module(cpp_utils_mod cpp_utils_mod.cpp)
target_link_libraries(cpp_utils_mod mcpib)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/cpp_utils_test.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_test(cpp_utils_test ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/cpp_utils_test.py")
