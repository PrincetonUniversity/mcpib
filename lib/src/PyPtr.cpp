/*
 * Copyright (c) 2010-2012, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/PyPtr.hpp"

namespace mcpib {

PyPtr PyPtr::getattr(char const * name, PyPtr default_) const {
    PyPtr result = steal(PyObject_GetAttrString(_p, name));
    if (!result) {
        result = default_;
        PyErr_Clear();
    }
    return result;
}

} // namespace mcpib
