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

void declareWrapperErrors(PyPtr const & module);

} // namespace mcpib

#endif // !MCPIB_WrapperError_hpp_INCLUDED
