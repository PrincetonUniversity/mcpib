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

PyTypeObject metatype = {
    PyVarObject_HEAD_INIT(NULL, 0)
    const_cast<char*>("mcpib.Type"),     // tp_name
    0, // set in declareClassTypes       // tp_basicsize
    0,                                   // tp_itemsize
    0,                                   // tp_dealloc
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
    "Python metatype for mcpib-wrapped C++ classes",   // tp_doc
    0,                                   // tp_traverse
    0,                                   // tp_clear
    0,                                   // tp_richcompare
    0,                                   // tp_weaklistoffset
    0,                                   // tp_iter
    0,                                   // tp_iternext
    0,                                   // tp_methods
    0,                                   // tp_members
    0,                                   // tp_getset
    0, // set in declareClassWrappers    // tp_base
    0,                                   // tp_dict
    0,                                   // tp_descr_get
    0,                                   // tp_descr_set
    0,                                   // tp_dictoffset
    0,                                   // tp_init
    0,                                   // tp_alloc
    0,                                   // tp_new
    0,                                   // tp_free
    0, // set in declareClassWrappers    // tp_is_gc
    0,                                   // tp_bases
    0,                                   // tp_mro
    0,                                   // tp_subclasses
    0,                                   // tp_cache
    0,                                   // tp_weaklist
};

PyTypeObject PyInstance::type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    const_cast<char*>("mcpib.Object"),    // tp_name
    sizeof(PyInstance),                  // tp_basicsize
    0,                                   // tp_itemsize
    (destructor)PyInstance::destroy,     // tp_dealloc
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
    "Base class for mcpib-wrapped C++ classes",   // tp_doc
    0,                                   // tp_traverse
    0,                                   // tp_clear
    0,                                   // tp_richcompare
    0,                                   // tp_weaklistoffset
    0,                                   // tp_iter
    0,                                   // tp_iternext
    0,                                   // tp_methods
    0,                                   // tp_members
    0,                                   // tp_getset
    0,                                   // tp_base
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

    explicit InstanceFromPythonConverter(PyPtr const & p) : FromPythonConverter(0), _p(p) {}

    virtual void * convert() {
        return reinterpret_cast<PyInstance*>(_p.get())->holder->get();
    }

private:
    PyPtr _p;
};

class InstanceFromPythonFactory: public FromPythonFactory {
public:

    explicit InstanceFromPythonFactory(PyPtr const & cls, std::string const & name) :
        FromPythonFactory(name, true, false),
        _cls(cls)
    {}

    virtual std::unique_ptr<FromPythonConverter> apply(PyPtr const & p) const {
        if (Py_TYPE(p.get()) == reinterpret_cast<PyTypeObject*>(_cls.get()))  {
            return std::unique_ptr<FromPythonConverter>(new InstanceFromPythonConverter(p));
        }
        return nullptr;
    }

    virtual std::unique_ptr<FromPythonFactory> clone(TypeRegistry & registry) const {
        return std::unique_ptr<FromPythonFactory>(new InstanceFromPythonFactory(_cls, name));
    }

private:
    PyPtr _cls;
};

} // anonymous;

ClassBase::ClassBase(std::string const & name) {
    PyPtr py_name = PyPtr::steal(PyString_FromString(name.c_str()));
    PyPtr py_bases = PyPtr::steal(PyTuple_New(1));
    Py_INCREF(&PyInstance::type);
    PyTuple_SET_ITEM(py_bases.get(), 0, reinterpret_cast<PyObject*>(&PyInstance::type));
    PyPtr py_dict = PyPtr::steal(PyDict_New());
    _py = PyPtr::steal(
        PyObject_CallFunctionObjArgs(
            reinterpret_cast<PyObject*>(&metatype),
            py_name.get(), py_bases.get(), py_dict.get(), nullptr
        )
    );
}

PyPtr ClassBase::makeInstance(std::unique_ptr<Holder> holder) const {
    PyTypeObject * type = _getPyTypeObject();
    PyPtr result = PyPtr::steal(type->tp_alloc(type, 0));
    PyInstance * instance = reinterpret_cast<PyInstance*>(result.get());
    instance->holder = holder.release();
    return result;
}

void ClassBase::_attachTo(Module & module, TypeInfo const & ctype) {
    // Set the __module__ attribute.
    char const * module_name = PyModule_GetName(module._py.get());
    PyPtr py_module_name = PyPtr::steal(PyString_FromString(module_name));
    PyObject_SetAttrString(_py.get(), "__module__", py_module_name.get());

    // Actually add the class object to the module.
    PyModule_AddObject(module._py.get(), _getPyTypeObject()->tp_name, _py.incref());

    // Register from-Python converters for the new class.
    std::string full_name = fmt::format("{}.{}", module_name, _getPyTypeObject()->tp_name);
    std::unique_ptr<FromPythonFactory> fromPython(new InstanceFromPythonFactory(_py, full_name));
    module.getRegistry().require(ctype)->registerFromPython(std::move(fromPython));
}

namespace internal {

void declareClassTypes(PyPtr const & module) {
    {
        Py_TYPE(&metatype) = &PyType_Type;
        metatype.tp_base = &PyType_Type;
        metatype.tp_basicsize = PyType_Type.tp_basicsize;
        metatype.tp_is_gc = PyType_Type.tp_is_gc;
        if (PyType_Ready(&metatype) != 0) return;
        auto r = PyPtr::borrow(reinterpret_cast<PyObject*>(&metatype));
        PyModule_AddObject(module.get(), "Type", r.release());
    }
    {
        Py_TYPE(&PyInstance::type) = &metatype;
        if (PyType_Ready(&PyInstance::type) != 0) return;
        auto r = PyPtr::borrow(reinterpret_cast<PyObject*>(&PyInstance::type));
        PyModule_AddObject(module.get(), "Object", r.release());
    }
}

} // namespace internal

} // namespace mcpib
