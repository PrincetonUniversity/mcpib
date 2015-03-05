
python_add_module(cpp_utils_mod cpp_utils_mod.cpp)
target_link_libraries(cpp_utils_mod mcpib)

function(add_python_test NAME)
  add_custom_target(
    ${NAME}.py ALL
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.py
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.py
    ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.py
  )
  add_test(${NAME} ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/cpp_utils_test.py")
endfunction(add_python_test)

add_python_test(cpp_utils_test)
add_python_test(exceptions_test)
