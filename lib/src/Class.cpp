/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/Class.hpp"
#include "mcpib/Holder.hpp"
#include "mcpib/WrapperError.hpp"
#include "mcpib/Module.hpp"
#include "mcpib/internal/format.h"
#include "mcpib/internal/initializers.hpp"
#include "mcpib/internal/trace.hpp"

#include <limits>

namespace mcpib {

namespace {

struct PyInstance {

    static PyTypeObject type;

    static void destroy(PyObject * self);

    PyInstance(PyInstance const &) = delete;
    PyInstance(PyInstance &&) = delete;

    PyInstance & operator=(PyInstance const &) = delete;
    PyInstance & operator=(PyInstance &&) = delete;

    PyObject base;
    Holder * holder;
};

static_assert(std::is_standard_layout<PyInstance>::value, "PyInstance is not standard layout.");

void PyInstance::destroy(PyObject * self) {
    internal::trace<2>("destroy: self={}, refcnt={}", (void*)self, self->ob_refcnt);
    delete reinterpret_cast<PyInstance*>(self)->holder;
    self->ob_type->tp_free(self);
}

void destroyPyClass(PyClass * self) {
    delete self->ctype;
    delete [] self->base.tp_name;
}

PyTypeObject metaclass = {
    PyVarObject_HEAD_INIT(NULL, 0)
    const_cast<char*>("mcpib.Class"),    // tp_name
    sizeof(PyClass),                     // tp_basicsize
    0,                                   // tp_itemsize
    (destructor)&destroyPyClass,         // tp_dealloc
    0,                                   // tp_print
    0,                                   // tp_getattr
    0,                                   // tp_setattr
    0,                                   // tp_compare
    0,                                   // tp_repr
    0,                                   // tp_as_number
    0,                                   // tp_as_sequence
    0,                                   // tp_as_mapping
    0,                                   // tp_hash
    0,                                   // tp_call
    0,                                   // tp_str
    0,                                   // tp_getattro
    0,                                   // tp_setattro
    0,                                   // tp_as_buffer
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   // tp_flags
    "Python metaclass for mcpib-wrapped C++ classes",   // tp_doc
    0,                                   // tp_traverse
    0,                                   // tp_clear
    0,                                   // tp_richcompare
    0,                                   // tp_weaklistoffset
    0,                                   // tp_iter
    0,                                   // tp_iternext
    0,                                   // tp_methods
    0,                                   // tp_members
    0,                                   // tp_getset
    0, // &PyType_Type                   // tp_base
    0,                                   // tp_dict
    0,                                   // tp_descr_get
    0,                                   // tp_descr_set
    0,                                   // tp_dictoffset
    0,                                   // tp_init
    0,                                   // tp_alloc
    0,                                   // tp_new
    0,                                   // tp_free
    0,                                   // tp_is_gc
    0,                                   // tp_bases
    0,                                   // tp_mro
    0,                                   // tp_subclasses
    0,                                   // tp_cache
    0,                                   // tp_weaklist
};

class InstanceFromPythonConverter : public FromPythonConverter {
public:

    InstanceFromPythonConverter(PyPtr const & p, Penalty penalty) : FromPythonConverter(penalty), _p(p) {}

    virtual void * convert() {
        return reinterpret_cast<PyInstance*>(_p.get())->holder->get();
    }

private:
    PyPtr _p;
};

class InstanceFromPythonFactory: public FromPythonFactory {
public:

    explicit InstanceFromPythonFactory(PyClass * cls, std::string const & name) :
        FromPythonFactory(name, true, false),
        _cls(cls)
    {}

    virtual std::unique_ptr<FromPythonConverter> apply(PyPtr const & p) const {
        if (Py_TYPE(p.get()) == &_cls->base)  {
            return std::unique_ptr<FromPythonConverter>(new InstanceFromPythonConverter(p, 0));
        }
        if (Py_TYPE(Py_TYPE(p.get())) != &metaclass) {
            return nullptr;
        }
        // Following the implementation of PyType_IsSubtype, we use the MRO to check all base classes
        // without recursion.
        PyObject * mro = Py_TYPE(p.get())->tp_mro;
        assert(PyTuple_Check(mro));
        Py_ssize_t size = PyTuple_GET_SIZE(mro);
        for (Py_ssize_t i = 0; i < size; i++) {
            if (PyTuple_GET_ITEM(mro, i) == reinterpret_cast<PyObject*>(_cls)) {
                return std::unique_ptr<FromPythonConverter>(new InstanceFromPythonConverter(p, i));
            }
        }
        return nullptr;
    }

    virtual std::unique_ptr<FromPythonFactory> clone(TypeRegistry & registry) const {
        return std::unique_ptr<FromPythonFactory>(new InstanceFromPythonFactory(_cls, name));
    }

private:
    PyClass * _cls;
};

} // anonymous;

PyClass PyClass::make(TypeInfo const & ctype) {
    PyClass result = {
        {
            PyVarObject_HEAD_INIT(NULL, 0)
            0,                                   // tp_name
            sizeof(PyInstance),                  // tp_basicsize
            0,                                   // tp_itemsize
            (destructor)&PyInstance::destroy,    // tp_dealloc
            0,                                   // tp_print
            0,                                   // tp_getattr
            0,                                   // tp_setattr
            0,                                   // tp_compare
            0,                                   // tp_repr
            0,                                   // tp_as_number
            0,                                   // tp_as_sequence
            0,                                   // tp_as_mapping
            0,                                   // tp_hash
            0,                                   // tp_call
            0,                                   // tp_str
            0,                                   // tp_getattro
            0,                                   // tp_setattro
            0,                                   // tp_as_buffer
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   // tp_flags
            "Python base class for mcpib-wrapped C++ classes",   // tp_doc
            0,                                   // tp_traverse
            0,                                   // tp_clear
            0,                                   // tp_richcompare
            0,                                   // tp_weaklistoffset
            0,                                   // tp_iter
            0,                                   // tp_iternext
            0,                                   // tp_methods
            0,                                   // tp_members
            0,                                   // tp_getset
            0, // PyBaseObject_Type              // tp_base
            0,                                   // tp_dict
            0,                                   // tp_descr_get
            0,                                   // tp_descr_set
            0,                                   // tp_dictoffset
            0,                                   // tp_init
            0,                                   // tp_alloc
            0,                                   // tp_new
            0,                                   // tp_free
        }
    };
    result.base.ob_type = &metaclass;
    result.ctype = new TypeInfo(ctype);
    return result;
}

PyPtr PyClass::makeInstance(std::unique_ptr<Holder> holder) {
    PyPtr result = PyPtr::steal(base.tp_alloc(&base, 0));
    PyInstance * instance = reinterpret_cast<PyInstance*>(result.get());
    instance->holder = holder.release();
    return result;
}

PyPtr ClassBase::_ready(Module & module) {
    char const * module_name = PyModule_GetName(module._py.get());
    if (!module_name) {
        return nullptr;
    }
    std::string full = fmt::format("{}.{}", module_name, _name);
    char * buf = new char[full.size() + 1];
    std::strncpy(buf, full.data(), full.size());
    delete [] _py->base.tp_name;
    _py->base.tp_name = buf;
    if (PyType_Ready(reinterpret_cast<PyTypeObject*>(_py)) < 0) {
        return nullptr;
    }
    std::unique_ptr<FromPythonFactory> fromPython(new InstanceFromPythonFactory(_py, full));
    module.getRegistry().require(*_py->ctype)->registerFromPython(std::move(fromPython));
    return PyPtr::borrow(reinterpret_cast<PyObject*>(_py));
}

namespace internal {

void declareClassTypes(PyPtr const & module) {
    Py_TYPE(&metaclass) = &PyType_Type;
    metaclass.tp_base = &PyType_Type;
    if (PyType_Ready(&metaclass) != 0) return;
    auto r = PyPtr::borrow(reinterpret_cast<PyObject*>(&metaclass));
    PyModule_AddObject(module.get(), "Class", r.release());
}

} // namespace internal

} // namespace mcpib
