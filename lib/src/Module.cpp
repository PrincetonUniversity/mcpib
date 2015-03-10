/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/Module.hpp"
#include "mcpib/WrapperError.hpp"

namespace mcpib {

static char const * const REGISTRY_ATTRIBUTE = "__mcpib_registry__";

Module::Module(char const * name, char const * doc) :
    _py(PyPtr::borrow(Py_InitModule3(name, nullptr, doc))),
    _registry()
{
    // import the mcpib module to ensure its Python type objects have been initialized.
    PyPtr::steal(PyImport_ImportModule("mcpib"));
    _registry._py = TypeRegistry::_make();
    PyModule_AddObject(_py.get(), REGISTRY_ATTRIBUTE, _registry._py.incref());
}

Module::Module(PyPtr const & py) :
    _py(py),
    _registry()
{
    _registry._py = PyPtr::steal(PyObject_GetAttrString(_py.get(), REGISTRY_ATTRIBUTE));
    _registry._checkPyType();
}

} // namespace mcpib
