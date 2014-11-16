/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_numeric_hpp_INCLUDED
#define MCPIB_numeric_hpp_INCLUDED

#include "mcpib/TypeRegistration.hpp"

namespace mcpib { namespace builtin {

void registerNumericConverters(TypeRegistry & registry);

}} // namespace mcpib::builtin

#endif // !MCPIB_numeric_hpp_INCLUDED
