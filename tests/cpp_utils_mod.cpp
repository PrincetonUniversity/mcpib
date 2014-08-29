/*
 * Copyright (c) 2010-2012, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/PyPtr.hpp"

static ::PyObject * testPyPtr(::PyObject * self, ::PyObject *) {
    Py_RETURN_TRUE;
}

static ::PyMethodDef methods[] = {
    "testPyPtr", testPyPtr, METH_NOARGS, "tests for the PyPtr class",
    {nullptr, nullptr, 0, nullptr}
};

PyMODINIT_FUNC
initcpp_utils_mod(void) {
    PyObject * m = Py_InitModule("cpp_utils_mod", methods);
    if (!m) return;
}
