/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_FromPythonConverter_hpp_INCLUDED
#define MCPIB_FromPythonConverter_hpp_INCLUDED

#include "mcpib/PyPtr.hpp"
#include "mcpib/TypeInfo.hpp"

#include <memory>
#include <vector>

namespace mcpib {

typedef unsigned int Penalty;

/*
 * Base class for all from-Python converters.
 *
 * Derived classes of FromPythonConverter must be created via a free function or static member function
 * that matches the signature of the FromPythonFactory typedef - that is, it takes a single PyPtr
 * argument, and returns a unique_ptr<FromPythonConverter>.  The returned pointer should be null
 * if no conversion is possible.  If a conversion is possible, the factory function should pass
 * whatever information is necessary to complete the conversion (usually at least the PyPtr itself)
 * to the derived converter's constructor.
 *
 * Derived class constructors must pass an integer penalty value to the base class constructor, which
 * indicates how appropriate the converter is for the Python object we're attempting to convert.  A
 * penalty of zero indicates a perfect match, with higher numbers indicating implicit conversions
 * (including conversions from a Python object that holds a derived classes when looking for a base
 * class reference or pointer; each level of inheritance should correspond to one unit of penalty).
 * When converting types for overloaded functions, the maximum penalty value of all arguments is
 * used to compare different overloads to detemrine which one will be called.
 *
 * All derived classes must implement convert() to actually carry out the conversion, and provide
 * a non-trivial destructor if needed to clean up the pointer returned by convert().  A postcall()
 * method may also be provided, and will be called after the function that used the converter is
 * called.
 */
class FromPythonConverter {
public:

    explicit FromPythonConverter(Penalty penalty_) : penalty(penalty_) {}

    Penalty const penalty;

    virtual void * convert() = 0;

    virtual void postcall() {}

    virtual ~FromPythonConverter() {}

};

typedef std::unique_ptr<FromPythonConverter> (*FromPythonFactory)(PyPtr const &);

typedef std::vector<std::unique_ptr<FromPythonConverter>> ConverterVector;


template <typename T>
class AdaptFromPython {
public:
    static bool const is_lvalue = false;
    static TypeInfo getTypeInfo() { return makeTypeInfo<T>(); }
    static T adapt(void * converted) { return *reinterpret_cast<T*>(converted); }
};

template <typename U>
class AdaptFromPython<U const> {
public:
    static bool const is_lvalue = false;
    static TypeInfo getTypeInfo() { return makeTypeInfo<U>(); }
    static U const adapt(void * converted) { return *reinterpret_cast<U const *>(converted); }
};

template <typename U>
class AdaptFromPython<U *> {
public:
    static bool const is_lvalue = true;
    static TypeInfo getTypeInfo() { return makeTypeInfo<U>(); }
    static U * adapt(void * converted) { return reinterpret_cast<U *>(converted); }
};

template <typename U>
class AdaptFromPython<U &> {
public:
    static bool const is_lvalue = true;
    static TypeInfo getTypeInfo() { return makeTypeInfo<U>(); }
    static U & adapt(void * converted) { return *reinterpret_cast<U *>(converted); }
};

template <typename U>
class AdaptFromPython<U const *> {
public:
    static bool const is_lvalue = false;
    static TypeInfo getTypeInfo() { return makeTypeInfo<U>(); }
    static U const * adapt(void * converted) { return reinterpret_cast<U const *>(converted); }
};

template <typename U>
class AdaptFromPython<U const &> {
public:
    static bool const is_lvalue = false;
    static TypeInfo getTypeInfo() { return makeTypeInfo<U>(); }
    static U const & adapt(void * converted) { return *reinterpret_cast<U const *>(converted); }
};

} // namespace mcpib

#endif // !MCPIB_FromPythonConverter_hpp_INCLUDED
