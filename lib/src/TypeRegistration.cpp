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
    bool is_lvalue,
    bool is_pointer
) const {
    std::unique_ptr<FromPythonConverter> converter;
    for (auto iter = _from_python.rbegin(); iter != _from_python.rend(); ++iter) {
        // If the type is an lvalue, we must have an lvalue converter.
        // But if the converter is only for pointers, the type must be a pointer.
        if ((!is_lvalue || (**iter).is_lvalue) && (is_pointer || !(**iter).is_pointer)) {
            converter = (**iter).apply(p);
            if (converter) break;
        }
    }
    return converter;
}

void TypeRegistration::setValueToPython(std::unique_ptr<ToPythonConverter> converter) {
    _value_to_python = std::move(converter);
}

void TypeRegistration::setRefToPython(std::unique_ptr<ToPythonConverter> converter) {
    _ref_to_python = std::move(converter);
}

void TypeRegistration::setConstRefToPython(std::unique_ptr<ToPythonConverter> converter) {
    _const_ref_to_python = std::move(converter);
}

void TypeRegistration::setPointerToPython(std::unique_ptr<ToPythonConverter> converter) {
    _pointer_to_python = std::move(converter);
}

void TypeRegistration::setConstPointerToPython(std::unique_ptr<ToPythonConverter> converter) {
    _const_pointer_to_python = std::move(converter);
}

void TypeRegistration::_copyInto(TypeRegistration & other, TypeRegistry & registry) const {
    for (auto const & other_conv : _from_python) {
        other.registerFromPython(other_conv->clone(registry));
    }
    if (_value_to_python) other.setValueToPython(_value_to_python->clone(registry));
    if (_ref_to_python) other.setRefToPython(_ref_to_python->clone(registry));
    if (_const_ref_to_python) other.setConstRefToPython(_const_ref_to_python->clone(registry));
    if (_pointer_to_python) other.setPointerToPython(_pointer_to_python->clone(registry));
    if (_const_pointer_to_python) other.setConstPointerToPython(_const_pointer_to_python->clone(registry));
    for (auto const & pair : _derived) {
        other._derived[pair.first] = registry.require(pair.first);
    }
}

ToPythonConverter const & TypeRegistration::getValueToPython() const {
    if (!_value_to_python) {
        // TODO: include type string
        throw raiseToPythonError("No move/copy to-Python converter found");
    }
    return *_value_to_python;
}

ToPythonConverter const & TypeRegistration::getRefToPython() const {
    if (!_ref_to_python) {
        // TODO: include type string
        throw raiseToPythonError("No reference to-Python converter found");
    }
    return *_ref_to_python;
}

ToPythonConverter const & TypeRegistration::getConstRefToPython() const {
    if (!_const_ref_to_python) {
        // TODO: include type string
        throw raiseToPythonError("No const reference to-Python converter found");
    }
    return *_const_ref_to_python;
}

ToPythonConverter const & TypeRegistration::getPointerToPython() const {
    if (_pointer_to_python) {
        return *_pointer_to_python;
    } else if (_ref_to_python) {
        return *_ref_to_python;
    }
    // TODO: include type string
    throw raiseToPythonError("No pointer to-Python converter found");
}

ToPythonConverter const & TypeRegistration::getConstPointerToPython() const {
    if (_const_pointer_to_python) {
        return *_const_pointer_to_python;
    } else if (_const_ref_to_python) {
        return *_const_ref_to_python;
    }
    // TODO: include type string
    throw raiseToPythonError("No const pointer to-Python converter found");
}

} // namespace mcpib
