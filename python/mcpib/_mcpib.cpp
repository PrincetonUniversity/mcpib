/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/internal/initializers.hpp"

static PyMethodDef methods[] = {
    {nullptr, nullptr, 0, nullptr}
};

PyMODINIT_FUNC
init_mcpib(void) {
    PyObject * m = Py_InitModule("_mcpib", methods);
    auto module = mcpib::PyPtr::borrow(m);
    mcpib::internal::declareWrapperErrors(module);
    mcpib::internal::declareCallableType(module);
}
