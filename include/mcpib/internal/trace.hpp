/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_INTERNAL_trace_hpp_INCLUDED
#define MCPIB_INTERNAL_trace_hpp_INCLUDED

#include "mcpib/PyPtr.hpp"

#ifdef MCPIB_TRACE
#include "mcpib/internal/format.h"
#include <iostream>
#endif

namespace mcpib { namespace internal {

#ifdef MCPIB_TRACE

template <int N, typename ...Args>
void trace(Args&&... args) {
    if (N <= MCPIB_TRACE) {
        fmt::print(std::cerr, args...);
        std::cerr << std::endl;
    }
}

#else

template <int N, typename ...Args>
void trace(Args&&... args) {}

#endif

}} // namespace mcpib::internal

#endif // !MCPIB_INTERNAL_trace_hpp_INCLUDED
