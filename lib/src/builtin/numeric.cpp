/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/builtin/numeric.hpp"
#include "mcpib/FromPython.hpp"

#include <climits>

namespace mcpib { namespace builtin {

namespace {

template <typename Derived, typename Target, typename Intermediate>
class IntegerFromPythonConverter : public FromPythonConverter {
public:

    static std::unique_ptr<FromPythonConverter> make(PyPtr const & p) {
        if (Derived::check(p)) {
            return std::unique_ptr<FromPythonConverter>(new Derived(p, 0));
        } else {
            return std::unique_ptr<FromPythonConverter>();
        }
    }

    static void declare(TypeRegistry & registry) {
        registry.require(makeTypeInfo<Target>())->registerFromPython(&make, false);
    }

    explicit IntegerFromPythonConverter(PyPtr const & p, Penalty penalty) :
        FromPythonConverter(penalty),
        _ptr(p)
    {}

    virtual void * convert() {
        Intermediate value = Derived::convertIntermediate(_ptr);
        if (PyErr_Occurred()) {
            // TODO: error handling
        }
        _value = value;
        if (static_cast<Intermediate>(_value) != value) {
            // TODO: overflow error
        }
        return &_value;
    }

private:
    PyPtr _ptr;
    Target _value;
};


class BoolFromPythonConverter :
    public IntegerFromPythonConverter<BoolFromPythonConverter,bool,bool> {
public:

    static bool check(PyPtr const & p) { return PyBool_Check(p.get()); }

    static bool convertIntermediate(PyPtr const & p) { return p.get() == Py_True; }

    // TODO: inherit ctor
    BoolFromPythonConverter(PyPtr const & p, Penalty penalty) :
        IntegerFromPythonConverter<BoolFromPythonConverter,bool,bool>(p, penalty)
    {}

};


template <typename T>
class LongFromPythonConverter :
    public IntegerFromPythonConverter<LongFromPythonConverter<T>,T,long long> {
public:

    static bool check(PyPtr const & p) { return PyInt_Check(p.get()) || PyLong_Check(p.get()); }

    static long long convertIntermediate(PyPtr const & p) {
        return PyLong_AsLongLong(p.get());
    }

    // TODO: inherit ctor
    LongFromPythonConverter(PyPtr const & p, Penalty penalty) :
        IntegerFromPythonConverter<LongFromPythonConverter<T>,T,long long>(p, penalty)
    {}

};


template <typename T>
class UnsignedLongFromPythonConverter :
    public IntegerFromPythonConverter<UnsignedLongFromPythonConverter<T>,T,unsigned long long> {
public:

    static bool check(PyPtr const & p) { return PyInt_Check(p.get()) || PyLong_Check(p.get()); }

    static unsigned long long convertIntermediate(PyPtr const & p) {
        return PyLong_AsUnsignedLongLong(p.get());
    }

    // TODO: inherit ctor
    UnsignedLongFromPythonConverter(PyPtr const & p, Penalty penalty) :
        IntegerFromPythonConverter<UnsignedLongFromPythonConverter<T>,T,unsigned long long>(p, penalty)
    {}

};


template <typename Target>
class FloatFromPythonConverter : public FromPythonConverter {
public:

    static std::unique_ptr<FromPythonConverter> make(PyPtr const & p) {
        if (PyFloat_Check(p.get())) {
            return std::unique_ptr<FromPythonConverter>(new FloatFromPythonConverter<Target>(p, 0));
        } else if (PyInt_Check(p.get()) || PyLong_Check(p.get())) {
            PyPtr p2 = PyPtr::borrow(PyNumber_Float(p.get()));
            return std::unique_ptr<FromPythonConverter>(new FloatFromPythonConverter<Target>(p2, 1));
        } else {
            return std::unique_ptr<FromPythonConverter>();
        }
    }

    static void declare(TypeRegistry & registry) {
        registry.require(makeTypeInfo<Target>())->registerFromPython(&make, false);
    }

    explicit FloatFromPythonConverter(PyPtr const & p, Penalty penalty) :
        FromPythonConverter(penalty),
        _ptr(p)
    {}

    virtual void * convert() {
        _value = PyFloat_AsDouble(_ptr.get());
        if (PyErr_Occurred()) {
            // TODO: handle error
        }
        return &_value;
    }

private:
    PyPtr _ptr;
    Target _value;
};


} // anonymous

void registerNumericConverters(TypeRegistry & registry) {
    BoolFromPythonConverter::declare(registry);
#if CHAR_MIN < 0
    LongFromPythonConverter<char>::declare(registry);
#else
    UnsignedLongFromPythonConverter<char>::declare(registry);
#endif
    LongFromPythonConverter<signed char>::declare(registry);
    LongFromPythonConverter<short>::declare(registry);
    LongFromPythonConverter<int>::declare(registry);
    LongFromPythonConverter<long>::declare(registry);
    LongFromPythonConverter<long long>::declare(registry);
    UnsignedLongFromPythonConverter<unsigned char>::declare(registry);
    UnsignedLongFromPythonConverter<unsigned short>::declare(registry);
    UnsignedLongFromPythonConverter<unsigned int>::declare(registry);
    UnsignedLongFromPythonConverter<unsigned long>::declare(registry);
    UnsignedLongFromPythonConverter<unsigned long long>::declare(registry);
    FloatFromPythonConverter<float>::declare(registry);
    FloatFromPythonConverter<double>::declare(registry);
}

}} // namespace mcpib::builtin
