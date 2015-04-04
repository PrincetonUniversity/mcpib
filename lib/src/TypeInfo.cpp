/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/TypeInfo.hpp"

#include <iostream>

#if __GNUC__
#include <cxxabi.h>
#define MCPIB_GCC_DEMANGLE
#endif

namespace mcpib {

#ifdef MCPIB_GCC_DEMANGLE

std::string TypeInfo::demangle() const {
    int status = 0;
    char * s = abi::__cxa_demangle(name(), nullptr, nullptr, &status);
    std::string r;
    if (status == 0) {
        r = s;
    } else {
        r = name();
        r += " (mangled)";
    }
    free(s);
    return r;
}

#else

std::string TypeInfo::demangle() const { return name(); }

#endif

std::ostream & operator<<(std::ostream & os, TypeInfo const & t) {
    return os << t.demangle();
}

} // namespace mcpib
