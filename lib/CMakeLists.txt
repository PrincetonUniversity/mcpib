add_library(mcpib SHARED
  src/builtin/numeric.cpp
  src/Callable.cpp
  src/CallableOverload.cpp
  src/PyPtr.cpp
  src/PythonException.cpp
  src/TypeInfo.cpp
  src/TypeRegistration.cpp
  src/TypeRegistry.cpp
  src/WrapperError.cpp
  )
