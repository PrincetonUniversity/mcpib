/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */


#include "mcpib/TypeRegistry.hpp"
#include "mcpib/WrapperError.hpp"
#include "mcpib/internal/initializers.hpp"
#include "mcpib/internal/trace.hpp"
#include "mcpib/internal/format.h"

#include <unordered_map>

namespace mcpib {

namespace {

struct PyTypeRegistry {

    static PyTypeObject type;

    static void destroy(PyObject * self);

    explicit PyTypeRegistry();

    std::shared_ptr<TypeRegistration> lookup(TypeInfo const & t) const;

    std::shared_ptr<TypeRegistration> require(TypeInfo const & t);

    void import(PyTypeRegistry const & other);

    PyObject base;
    TypeRegistration::Map map;
};

PyTypeObject PyTypeRegistry::type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    const_cast<char*>("mcpib.TypeRegistry"), // tp_name
    sizeof(PyTypeRegistry),                  // tp_basicsize
    0,                                       // tp_itemsize
    (destructor)&PyTypeRegistry::destroy,    // tp_dealloc
    0,                                       // tp_print
    0,                                       // tp_getattr
    0,                                       // tp_setattr
    0,                                       // tp_compare
    0,                                       // tp_repr
    0,                                       // tp_as_number
    0,                                       // tp_as_sequence
    0,                                       // tp_as_mapping
    0,                                       // tp_hash
    0,                                       // tp_call
    0,                                       // tp_str
    0,                                       // tp_getattro
    0,                                       // tp_setattro
    0,                                       // tp_as_buffer
    Py_TPFLAGS_DEFAULT,                      // tp_flags
    "Registry of C++ types known to module", // tp_doc
    0,                                       // tp_traverse
    0,                                       // tp_clear
    0,                                       // tp_richcompare
    0,                                       // tp_weaklistoffset
    0,                                       // tp_iter
    0,                                       // tp_iternext
    0,                                       // tp_methods
    0,                                       // tp_members
    0,                                       // tp_getset
    0,                                       // tp_base
    0,                                       // tp_dict
    0,                                       // tp_descr_get
    0,                                       // tp_descr_set
    0,                                       // tp_dictoffset
    0,                                       // tp_init
    0,                                       // tp_alloc
    0,                                       // tp_new
    0,                                       // tp_free
};

void PyTypeRegistry::destroy(PyObject * self) {
    delete reinterpret_cast<PyTypeRegistry*>(self);
}

PyTypeRegistry::PyTypeRegistry() {
    PyObject_Init(&base, &type);
}

std::shared_ptr<TypeRegistration> PyTypeRegistry::lookup(TypeInfo const & t) const {
    auto iter = map.find(t);
    if (iter == map.end()) {
        internal::trace<9>("TypeRegistry::lookup: search for {} unsuccessful.", t);
        return std::shared_ptr<TypeRegistration>();
    }
    internal::trace<9>("TypeRegistry::lookup: search for {} successful", t);
    return iter->second;
}

std::shared_ptr<TypeRegistration> PyTypeRegistry::require(TypeInfo const & t) {
    std::shared_ptr<TypeRegistration> & element = map[t];
    if (!element) {
        internal::trace<9>("TypeRegistry::require: search for {} unsuccessful; adding.", t);
        element = std::make_shared<TypeRegistration>();
    } else {
        internal::trace<9>("TypeRegistry::require: search for {} successful.", t);
    }
    return element;
}

void PyTypeRegistry::import(PyTypeRegistry const & other) {
}

}     // anonymous

namespace internal {

void declareTypeRegistryType(PyPtr const & module) {
    if (PyType_Ready(&PyTypeRegistry::type) != 0) return;
    auto r = PyPtr::borrow(reinterpret_cast<PyObject*>(&PyTypeRegistry::type));
    PyModule_AddObject(module.get(), "TypeRegistry", r.release());
}

} // namespace internal

PyPtr TypeRegistry::_make() {
    return PyPtr::steal(reinterpret_cast<PyObject*>(new PyTypeRegistry()));
}


void TypeRegistry::_checkPyType() {
    if (_py->ob_type != &PyTypeRegistry::type) {
        throw raiseWrapperError(
            fmt::format("Cannot construct TypeRegistry from Python object of type {}",
                        _py->ob_type->tp_name)
        );
    }
}


std::shared_ptr<TypeRegistration> TypeRegistry::lookup(TypeInfo const & t) const {
    return reinterpret_cast<PyTypeRegistry*>(_py.get())->lookup(t);
}

std::shared_ptr<TypeRegistration> TypeRegistry::require(TypeInfo const & t) {
    return reinterpret_cast<PyTypeRegistry*>(_py.get())->require(t);
}

std::shared_ptr<TypeRegistration> TypeRegistry::_lookupToPython(TypeInfo const & t) const {
    auto r = lookup(t);
    if (!r) {
        throw raiseToPythonError(fmt::format("No type registration found for '%s'", t.demangle()));
    }
    return r;
}


void TypeRegistry::_import(TypeRegistry const & other) {
    for (auto const & pair : reinterpret_cast<PyTypeRegistry*>(other._py.get())->map) {
        TypeInfo const & type = pair.first;
        std::shared_ptr<TypeRegistration> other_reg = pair.second;
        std::shared_ptr<TypeRegistration> new_reg = require(type);
        other_reg->_copyInto(*new_reg, *this);
    }
}

} // namespace mcpib
