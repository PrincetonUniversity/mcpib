/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_WrapperError_hpp_INCLUDED
#define MCPIB_WrapperError_hpp_INCLUDED

#include "mcpib/PythonException.hpp"

namespace mcpib {

PythonException raiseWrapperError(std::string message);
PythonException raiseUnknownCppException(std::string message);

// Thrown when failing to convert arguments to a function; inherits from TypeError and WrapperError
PythonException raiseSignatureError(std::string message);

// Thrown when one or more from-Python conversions fail, with no working alternative overload.
//  Inherits from SignatureError
PythonException raiseFromPythonError(std::string message);

// Thrown when there is a tie for which of a set of overloaded functions should be invoked.
PythonException raiseAmbiguousOverloadError(std::string message);

} // namespace mcpib

#endif // !MCPIB_WrapperError_hpp_INCLUDED
