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

PyPtr CallableOverloadBase::call(PyPtr const & args, PyPtr const & kwds) const {
    if (!PyTuple_Check(args.get())) {
        // TODO: better exception message
        PyErr_SetString(PyExc_TypeError, "positional arguments must be a tuple");
        return PyPtr();
    }
    Py_ssize_t const size = PyTuple_Size(args.get());
    if (static_cast<std::size_t>(size) != _arguments.size()) {
        // TODO: better exception message
        PyErr_SetString(PyExc_TypeError, "incorrect number of arguments");
    }
    ConverterVector converters(size);
    for (Py_ssize_t index = 0; index < size; ++index) {
        // TODO: keyword arguments
        PyPtr python_arg = PyPtr::borrow(PyTuple_GET_ITEM(args.get(), index));
        ArgumentData const & arg_data = _arguments[index];
        converters[index] = arg_data.registration->lookupFromPython(python_arg, arg_data.is_lvalue);
    }
    return _call(std::move(converters));
}

} // namespace mcpib
