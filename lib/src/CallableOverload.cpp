/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/CallableOverload.hpp"

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

CallableOverloadData::CallableOverloadData(
    PyPtr const & args, PyPtr const & kwds,
    CallableOverloadBase const * overload
) :
    _error_state(SUCCESS),
    _error_position(0),
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

    // TODO: handle keyword arguments
}

int CallableOverloadData::getPenalty() const {
    int penalty = 0;
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
