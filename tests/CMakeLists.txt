function(add_python_test NAME)
  add_custom_target(
    ${NAME}.py ALL
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.py
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.py
    ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.py
  )
  add_test(${NAME} ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.py")
endfunction(add_python_test)

function(add_dual_test NAME)
  python_add_module(${NAME}_mod ${NAME}_mod.cpp)
  target_link_libraries(${NAME}_mod mcpib)
  add_python_test(${NAME}_test)
  add_dependencies(${NAME}_test.py ${NAME}_mod)
endfunction(add_dual_test)

add_python_test(exceptions_test)
add_dual_test(cpp_utils)
add_dual_test(builtin_numeric)
