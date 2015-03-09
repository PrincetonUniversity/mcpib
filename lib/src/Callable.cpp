/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/Callable.hpp"
#include "mcpib/WrapperError.hpp"
#include "mcpib/internal/initializers.hpp"

#include <limits>

namespace mcpib {

namespace {

struct PyCallable {

    static PyTypeObject type;

    static PyObject * call(PyObject * self, PyObject * args, PyObject * kwds);

    static void destroy(PyObject * self);

    explicit PyCallable(std::string name);

    PyPtr call(PyPtr const & args, PyPtr const & kwds) const;

    PyObject base;
    std::string name;
    std::vector<std::unique_ptr<CallableOverloadBase>> overloads;
};

PyTypeObject PyCallable::type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    const_cast<char*>("mcpib.Callable"), // tp_name
    sizeof(PyCallable),                  // tp_basicsize
    0,                                   // tp_itemsize
    (destructor)&PyCallable::destroy,    // tp_dealloc
    0,                                   // tp_print
    0,                                   // tp_getattr
    0,                                   // tp_setattr
    0,                                   // tp_compare
    0,                                   // tp_repr
    0,                                   // tp_as_number
    0,                                   // tp_as_sequence
    0,                                   // tp_as_mapping
    0,                                   // tp_hash
    (ternaryfunc)&PyCallable::call,      // tp_call
    0,                                   // tp_str
    0,                                   // tp_getattro
    0,                                   // tp_setattro
    0,                                   // tp_as_buffer
    Py_TPFLAGS_DEFAULT,                  // tp_flags
    "C++ function or function object",   // tp_doc
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
};

PyObject * PyCallable::call(PyObject * self, PyObject * args, PyObject * kwds) {
    if (self->ob_type != &PyCallable::type) {
        raiseWrapperError("First argument must be a mcpib.Callable instance");
    }
    return reinterpret_cast<PyCallable*>(self)->call(PyPtr::borrow(args), PyPtr::borrow(kwds)).release();
}

void PyCallable::destroy(PyObject * self) {
    delete reinterpret_cast<PyCallable*>(self);
}

PyCallable::PyCallable(std::string name_) : name(std::move(name_)) {
    PyObject_Init(&base, &type);
}

PyPtr PyCallable::call(PyPtr const & args, PyPtr const & kwds) const {
    try {
        std::vector<CallableOverloadData> data;
        data.reserve(overloads.size());
        std::size_t best_index = -1;
        Penalty best_penalty = std::numeric_limits<unsigned int>::max();
        for (std::size_t i = 0; i < overloads.size(); ++i) {
            data.emplace_back(args, kwds, overloads[i].get());
            if (data[i].getState() == CallableOverloadData::SUCCESS) {
                Penalty new_penalty = data[i].getPenalty();
                if (new_penalty < best_penalty) {
                    best_index = i;
                    best_penalty = new_penalty;
                    if (best_penalty == 0) break;
                }
            }
        }
        if (best_index < 0) {
            // no matching overload; need to raise an exception
        }
        return data[best_index].call();
    } catch (PythonException & err) {
        return err.restore();
    } catch (std::exception & err) {
        return raiseUnknownCppException(err.what()).restore();
    } catch (...) {
        return raiseUnknownCppException("Unexpected non-exception C++ throw.").restore();
    }
}

} // anonymous

namespace internal {

void declareCallableType(PyPtr const & module) {
    if (PyType_Ready(&PyCallable::type) != 0) return;
    auto r = PyPtr::borrow(reinterpret_cast<PyObject*>(&PyCallable::type));
    PyModule_AddObject(module.get(), "Callable", r.release());
}

} // namespace internal

Callable::Callable(std::string name) :
    _py(PyPtr::steal(reinterpret_cast<PyObject*>(new PyCallable(std::move(name)))))
{}

Callable::Callable(PyPtr const & py) : _py(py) {
    if (py->ob_type != &PyCallable::type) {
        throw raiseWrapperError(
            std::string("Cannot construct Callable from Python object of type ")
            + std::string(py->ob_type->tp_name)
        );
    }
}

PyPtr Callable::call(PyPtr const & args, PyPtr const & kwds) const {
    auto self = reinterpret_cast<PyCallable*>(_py.get());
    return self->call(args, kwds);
}

void Callable::_addOverload(std::unique_ptr<CallableOverloadBase> && overload) {
    reinterpret_cast<PyCallable*>(_py.get())->overloads.push_back(std::move(overload));
}

} // namespace mcpib
