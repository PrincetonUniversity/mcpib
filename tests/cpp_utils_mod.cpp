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

static ::PyObject * testPyPtr(::PyObject * self, ::PyObject *) {

    PyPtr p1;
    if (p1) {
        Py_RETURN_FALSE;
    }

    ::PyObject * f = PyFloat_FromDouble(4.0);
    PyPtr p2 = PyPtr::steal(f);
    if (f != p2.get()) {
        Py_RETURN_FALSE;
    }
    if (f->ob_refcnt != 1) {
        Py_RETURN_FALSE;
    }

    p1 = p2; // p1 and p2 both hold f
    if (f != p1.get()) {
        Py_RETURN_FALSE;
    }
    if (f != p2.get()) {
        Py_RETURN_FALSE;
    }
    if (f->ob_refcnt != 2) {
        Py_RETURN_FALSE;
    }

    ::PyObject * g = PyFloat_FromDouble(6.1);
    PyPtr p3 = PyPtr::borrow(g); // p1 and p2 hold f; p3 holds g; also have an unmanaged ref to g
    if (!p3) {
        Py_RETURN_FALSE;
    }
    if (g != p3.get()) {
        Py_RETURN_FALSE;
    }
    if (g->ob_refcnt != 2) {
        Py_RETURN_FALSE;
    }

    swap(p3, p1);  // p1 holds g; p2 and p3 hold f; also have an unmanaged ref to g
    if (f != p3.get()) {
        Py_RETURN_FALSE;
    }
    if (g != p1.get()) {
        Py_RETURN_FALSE;
    }
    if (f->ob_refcnt != 2) {
        Py_RETURN_FALSE;
    }
    if (g->ob_refcnt != 2) {
        Py_RETURN_FALSE;
    }

    p1 = p2; // p1, p2, and p3 all hold f; also have unmanaged ref to g
    if (f != p1.get()) {
        Py_RETURN_FALSE;
    }
    if (f != p2.get()) {
        Py_RETURN_FALSE;
    }
    if (f->ob_refcnt != 3) {
        Py_RETURN_FALSE;
    }
    if (g->ob_refcnt != 1) {
        Py_RETURN_FALSE;
    }
    Py_DECREF(g); // delete unmanaged g ref

    ::PyObject * r = p1.release(); // p1 empty; p2 and p3 hold f; also have unmanaged ref to f in r
    if (f->ob_refcnt != 3) {
        Py_RETURN_FALSE;
    }
    if (p1) {
        Py_RETURN_FALSE;
    }
    if (f != r) {
        Py_RETURN_FALSE;
    }
    Py_DECREF(r); // delete unmanaged f ref

    ::PyObject * s = p2.incref(); // p1 empty; p2 and p3 hold f, also have unmanaged ref to f in s
    if (f->ob_refcnt != 3) {
        Py_RETURN_FALSE;
    }
    if (f != s) {
        Py_RETURN_FALSE;
    }
    Py_DECREF(s); // delete unmanaged f ref

    PyPtr func = p3.getattr("is_integer");

    return PyObject_CallObject(func.get(), nullptr);
}

static ::PyMethodDef methods[] = {
    "testPyPtr", testPyPtr, METH_NOARGS, "tests for the PyPtr class",
    {nullptr, nullptr, 0, nullptr}
};

} // mcpib

PyMODINIT_FUNC
initcpp_utils_mod(void) {
    PyObject * m = Py_InitModule("cpp_utils_mod", mcpib::methods);
    if (!m) return;
}
