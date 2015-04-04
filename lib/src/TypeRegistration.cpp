/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/TypeRegistration.hpp"
#include "mcpib/TypeRegistry.hpp"
#include "mcpib/WrapperError.hpp"

namespace mcpib {

void TypeRegistration::registerFromPython(FromPythonFactory factory, bool is_lvalue) {
    for (auto const & pair : _from_python) {
        if (pair.first == factory) {
            if (pair.second != is_lvalue) {
                throw raiseWrapperError(
                    "Identical from-python converter registered with different lvalue status"
                );
            }
            return;
        }
    }
    _from_python.push_back(std::make_pair(factory, is_lvalue));
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

void TypeRegistration::_copyInto(TypeRegistration & other, TypeRegistry & registry) const {
    for (auto const & pair : _from_python) {
        other.registerFromPython(pair.first, pair.second);
    }
    for (auto const & pair : _derived) {
        other._derived[pair.first] = registry.require(pair.first);
    }
}

} // namespace mcpib
