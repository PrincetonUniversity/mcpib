/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/PythonException.hpp"

namespace mcpib {

PythonException PythonException::fetch() {
    PyObject * type = nullptr;
    PyObject * value = nullptr;
    PyObject * traceback = nullptr;
    ::PyErr_Fetch(&type, &value, &traceback);
    ::PyErr_NormalizeException(&type, &value, &traceback);
    return PythonException(PyPtr::steal(type), PyPtr::steal(value), PyPtr::steal(traceback));
}

PythonException PythonException::raise(PyPtr const & type, std::string message) {
    ::PyErr_SetString(type.get(), message.c_str());
    return fetch();
}

std::nullptr_t PythonException::restore() {
    ::PyErr_Restore(_type.release(), _value.release(), _traceback.release());
    return nullptr;
}

char const * PythonException::what() const noexcept {
    if (_value) {
        return PyString_AS_STRING(_value.get());
    }
    return "";
}

PythonException::PythonException(PyPtr const & type, PyPtr const & value, PyPtr const & traceback) :
    _type(type), _value(value), _traceback(traceback)
{
    if (_value) {
        _str = PyPtr::steal(PyObject_Str(_value.get()));
    }
}


} // namespace mcpib
