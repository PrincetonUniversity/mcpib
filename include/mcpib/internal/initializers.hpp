/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_INTERNAL_initializers_hpp_INCLUDED
#define MCPIB_INTERNAL_initializers_hpp_INCLUDED

#include "mcpib/PyPtr.hpp"

namespace mcpib { namespace internal {

void declareWrapperErrors(PyPtr const & module);

void declareCallableType(PyPtr const & module);

}} // namespace mcpib::internal

#endif // !MCPIB_INTERNAL_initializers_hpp_INCLUDED
