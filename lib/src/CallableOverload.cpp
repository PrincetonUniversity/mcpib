/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/CallableOverload.hpp"
#include "mcpib/WrapperError.hpp"
#include "mcpib/internal/format.h"

#include <string>

namespace mcpib {

ArgumentDataBuilder::ArgumentDataBuilder(TypeRegistry & registry, std::initializer_list<std::string> names) :
    arguments(),
    _registry(&registry)
{
    arguments.reserve(names.size());
    for (auto iter = names.begin(); iter != names.end(); ++iter) {
        arguments.emplace_back(*iter);
    }
}

namespace {

Py_ssize_t findArgument(ArgumentDataVector const & arguments, char const * name) {
    for (std::size_t index = 0; index < arguments.size(); ++index) {
        if (arguments[index].name == name) {
            return index;
        }
    }
    return -1;
}

} // anonymous

CallableOverloadData::CallableOverloadData(
    PyPtr const & args, PyPtr const & kwds,
    CallableOverloadBase const * overload
) :
    _error_state(SUCCESS),
    _error_position(-1),
    _error_string(),
    _converters(PyTuple_Size(args.get())),
    _overload(overload)
{
    if (_converters.size() > _overload->_arguments.size()) {
        _error_state = TOO_MANY;
        return;
    }
    Py_ssize_t size = _converters.size();
    for (Py_ssize_t index = 0; index < size; ++index) {
        PyPtr python_arg = PyPtr::borrow(PyTuple_GET_ITEM(args.get(), index));
        ArgumentData const & arg_data = _overload->_arguments[index];
        _converters[index] = arg_data.registration->lookupFromPython(python_arg, arg_data.is_lvalue);
        if (!_converters[index]) {
            _error_state = NO_CONVERTER;
            _error_position = index;
            return;
        }
    }
    if (!kwds) return;
    PyObject * key = nullptr;
    PyObject * value = nullptr;
    Py_ssize_t kwd_index = 0;
    while (PyDict_Next(kwds.get(), &kwd_index, &key, &value)) {
        char const * name = PyString_AS_STRING(key);
        Py_ssize_t index = findArgument(_overload->_arguments, name);
        if (index < 0) {
            _error_state = UNKNOWN_KWARG;
            _error_string = name;
            return;
        }
        if (_converters[index]) {
            _error_state = DUPLICATE_ARG;
            _error_position = index;
        }
        ArgumentData const & arg_data = _overload->_arguments[index];
        _converters[index] = arg_data.registration->lookupFromPython(
            PyPtr::borrow(value),
            arg_data.is_lvalue
        );
        if (!_converters[index]) {
            _error_state = NO_CONVERTER;
            _error_position = index;
            return;
        }
    }
}

PyPtr CallableOverloadData::raiseException(std::string const & function_name) const {
    if (_error_state == TOO_MANY) {
        return raiseSignatureError(
            fmt::format("Too many arguments ({}) to function '{}' (expects at most {})",
                        _converters.size(), function_name, _overload->_arguments.size())
        ).restore();
    } else if (_error_state == NO_CONVERTER) {
        ArgumentData const & arg_data = _overload->_arguments[_error_position];
        // TODO: include C++ type information and Python object.
        return raiseFromPythonError(
            fmt::format("No converter found for argument '{}' to function '{}'",
                        arg_data.name, function_name)
        ).restore();
    }
    // TODO: handle keyword arguments
    return raiseFromPythonError(
        fmt::format("Cannot convert arguments to '{}'", function_name)
    ).restore();
}

Penalty CallableOverloadData::getPenalty() const {
    Penalty penalty = 0;
    for (auto iter = _converters.begin(); iter != _converters.end(); ++iter) {
        penalty = std::max((**iter).penalty, penalty);
    }
    return penalty;
}

PyPtr CallableOverloadData::call() const {
    PyPtr p = _overload->call(_converters);
    for (auto iter = _converters.begin(); iter != _converters.end(); ++iter) {
        (**iter).postcall();
    }
    return p;
}

} // namespace mcpib
