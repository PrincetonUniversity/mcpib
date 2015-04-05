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
#include "mcpib/Callable.hpp"

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

Module & Module::add(Callable const & callable) {
    PyModule_AddObject(_py.get(), callable.getName().c_str(), callable._py.incref());
    return *this;
}

Module & Module::add(std::string const & name, PyPtr const & value) {
    PyModule_AddObject(_py.get(), name.c_str(), value.incref());
    return *this;
}

Module Module::import(std::string const & module_name) {
    Module other(PyPtr::steal(PyImport_ImportModule(module_name.c_str())));
    _registry._import(other.getRegistry());
    return std::move(other);
}

} // namespace mcpib
