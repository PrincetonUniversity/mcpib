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

void TypeRegistration::registerFromPython(std::unique_ptr<FromPythonFactory> factory) {
    for (auto const & existing : _from_python) {
        if (factory->name == existing->name) {
            return;
        }
    }
    _from_python.push_back(std::move(factory));
}

std::unique_ptr<FromPythonConverter> TypeRegistration::lookupFromPython(
    PyPtr const & p,
    bool require_lvalue
) const {
    std::unique_ptr<FromPythonConverter> converter;
    for (auto iter = _from_python.rbegin(); iter != _from_python.rend(); ++iter) {
        if (!require_lvalue || (**iter).is_lvalue) {
            converter = (**iter).apply(p);
            if (converter) break;
        }
    }
    return converter;
}

void TypeRegistration::_copyInto(TypeRegistration & other, TypeRegistry & registry) const {
    for (auto const & other_conv : _from_python) {
        other.registerFromPython(other_conv->clone(registry));
    }
    for (auto const & pair : _derived) {
        other._derived[pair.first] = registry.require(pair.first);
    }
}

} // namespace mcpib
