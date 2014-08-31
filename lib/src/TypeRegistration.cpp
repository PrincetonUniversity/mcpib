/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/TypeRegistration.hpp"

namespace mcpib {

void TypeRegistration::registerFromPython(FromPythonFactory factory, bool is_value) {
    _from_python.push_back(std::make_pair(factory, is_value));
}

std::unique_ptr<FromPythonConverter> TypeRegistration::lookupFromPython(
    PyPtr const & p,
    bool require_lvalue
) const {
    std::unique_ptr<FromPythonConverter> converter;
    for (auto iter = _from_python.rbegin(); iter != _from_python.rend(); ++iter) {
        if (!require_lvalue || iter->second) {
            converter = (*iter->first)(p);
            if (converter) break;
        }
    }
    return converter;
}

} // mcpib
