/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/WrapperError.hpp"
#include "mcpib/internal/initializers.hpp"

namespace mcpib {

namespace {

static PyObject * WrapperError = nullptr;
static PyObject * UnknownCppException = nullptr;

void addException(
    PyPtr const & module, PyObject * & type,
    char const * name, char const * doc,
    PyObject * base=nullptr
) {
    type = PyErr_NewExceptionWithDoc(
        const_cast<char*>(name),
        const_cast<char*>(doc),
        base,
        nullptr  // dict
    );
    Py_INCREF(type);
    PyModule_AddObject(module.get(), name + 6, type);
}

} // anonymous

namespace internal {

void declareWrapperErrors(PyPtr const & module) {
    addException(module, WrapperError, "mcpib.WrapperError",
                 "Base class for all exceptions raised by mcpib wrapper code.");
    addException(module, UnknownCppException, "mcpib.UnknownCppException",
                 "Exception raised in Python when an unrecognized C++ exception is caught in C++.",
                 WrapperError);
}

} // namespace internal

PythonException raiseWrapperError(std::string message) {
    return PythonException::raise(PyPtr::borrow(WrapperError), std::move(message));
}

PythonException raiseUnknownCppException(std::string message) {
    return PythonException::raise(PyPtr::borrow(UnknownCppException), std::move(message));
}


} // namespace mcpib
