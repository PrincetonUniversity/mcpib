/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/Module.hpp"
#include "mcpib/FromPythonConverter.hpp"
#include "mcpib/PythonException.hpp"

#include <climits>

namespace mcpib {

namespace {

template <typename Derived, typename Target, typename Intermediate>
class IntegerFromPythonConverter : public FromPythonConverter {
public:

    class Factory : public FromPythonFactory {
    public:

        explicit Factory(std::string const & name) : FromPythonFactory(name, false) {}

        virtual std::unique_ptr<FromPythonConverter> apply(PyPtr const & py) const {
            if (Derived::check(py)) {
                return std::unique_ptr<FromPythonConverter>(new Derived(py, 0));
            } else {
                return std::unique_ptr<FromPythonConverter>();
            }
        }

        virtual std::unique_ptr<FromPythonFactory> clone(TypeRegistry & registry) const {
            return std::unique_ptr<FromPythonFactory>(new Factory(this->name));
        }

    };

    static void declare(std::string const & name, TypeRegistry & registry) {
        registry.require(makeTypeInfo<Target>())->registerFromPython(
            std::unique_ptr<FromPythonFactory>(new Factory(name))
        );
    }

    explicit IntegerFromPythonConverter(PyPtr const & p, Penalty penalty) :
        FromPythonConverter(penalty),
        _ptr(p)
    {}

    virtual void * convert() {
        Intermediate value = Derived::convertIntermediate(_ptr);
        if (PyErr_Occurred()) {
            throw PythonException::fetch();
        }
        _value = value;
        if (static_cast<Intermediate>(_value) != value) {
            throw PythonException::raise(
                PyPtr::borrow(PyExc_OverflowError),
                "Cannot convert " + std::to_string(value) + " to " + makeTypeInfo<Target>().demangle()
            );
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
        long long r;
        if (PyInt_Check(p)) {
            r = PyInt_AsLong(p.get());
        } else {
            r = PyLong_AsLongLong(p.get());
        }
        if (PyErr_Occurred()) {
            throw PythonException::fetch();
        }
        return r;
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
        unsigned long long r = 0;
        if (PyInt_Check(p)) {
            long tmp = PyInt_AsLong(p.get());
            if (tmp < 0) {
                throw PythonException::raise(
                    PyPtr::borrow(PyExc_OverflowError),
                    "Error converting negative integer to unsigned"
                );
            }
            r = tmp;
        } else {
            r = PyLong_AsUnsignedLongLong(p.get());
        }
        if (PyErr_Occurred()) {
            throw PythonException::fetch();
        }
        return r;
    }

    // TODO: inherit ctor
    UnsignedLongFromPythonConverter(PyPtr const & p, Penalty penalty) :
        IntegerFromPythonConverter<UnsignedLongFromPythonConverter<T>,T,unsigned long long>(p, penalty)
    {}

};


template <typename Target>
class FloatFromPythonConverter : public FromPythonConverter {
public:

    class Factory : public FromPythonFactory {
    public:

        explicit Factory(std::string const & name) : FromPythonFactory(name, false) {}

        virtual std::unique_ptr<FromPythonConverter> apply(PyPtr const & p) const {
            if (PyFloat_Check(p.get())) {
                return std::unique_ptr<FromPythonConverter>(new FloatFromPythonConverter<Target>(p, 0));
            } else if (PyInt_Check(p.get()) || PyLong_Check(p.get())) {
                PyPtr p2 = PyPtr::borrow(PyNumber_Float(p.get()));
                return std::unique_ptr<FromPythonConverter>(new FloatFromPythonConverter<Target>(p2, 1));
            } else {
                return std::unique_ptr<FromPythonConverter>();
            }
        }

        virtual std::unique_ptr<FromPythonFactory> clone(TypeRegistry & registry) const {
            return std::unique_ptr<FromPythonFactory>(new Factory(this->name));
        }

    };

    static void declare(std::string const & name, TypeRegistry & registry) {
        registry.require(makeTypeInfo<Target>())->registerFromPython(
            std::unique_ptr<FromPythonFactory>(new Factory(name))
        );
    }

    explicit FloatFromPythonConverter(PyPtr const & p, Penalty penalty) :
        FromPythonConverter(penalty),
        _ptr(p)
    {}

    virtual void * convert() {
        _value = PyFloat_AsDouble(_ptr.get());
        return &_value;
    }

private:
    PyPtr _ptr;
    Target _value;
};

} // anonymous
} // namespace mcpib


using namespace mcpib;

PyMODINIT_FUNC
initnumbers(void) {
    Module module("numbers");
    TypeRegistry & registry = module.getRegistry();
    BoolFromPythonConverter::declare("mcpib.numbers.bool", registry);
#if CHAR_MIN < 0
    LongFromPythonConverter<char>::declare("mcpib.numbers.char", registry);
#else
    UnsignedLongFromPythonConverter<char>::declare("mcpib.numbers.char", registry);
#endif
    LongFromPythonConverter<signed char>::declare("mcpib.numbers.schar", registry);
    LongFromPythonConverter<short>::declare("mcpib.numbers.short", registry);
    LongFromPythonConverter<int>::declare("mcpib.numbers.int", registry);
    LongFromPythonConverter<long>::declare("mcpib.numbers.long", registry);
    LongFromPythonConverter<long long>::declare("mcpib.numbers.longlong", registry);
    UnsignedLongFromPythonConverter<unsigned char>::declare("mcpib.numbers.uchar", registry);
    UnsignedLongFromPythonConverter<unsigned short>::declare("mcpib.numbers.ushort", registry);
    UnsignedLongFromPythonConverter<unsigned int>::declare("mcpib.numbers.uint", registry);
    UnsignedLongFromPythonConverter<unsigned long>::declare("mcpib.numbers.ulong", registry);
    UnsignedLongFromPythonConverter<unsigned long long>::declare("mcpib.numbers.ulonglong", registry);
    FloatFromPythonConverter<float>::declare("mcpib.numbers.float", registry);
    FloatFromPythonConverter<double>::declare("mcpib.numbers.double", registry);
    FloatFromPythonConverter<long double>::declare("mcpib.numbers.longdouble", registry);
}
