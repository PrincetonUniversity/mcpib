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

class TypeRegistry;

/*
 * Base class for all from-Python converters.
 *
 * Derived classes of FromPythonConverter are created via an instance of FromPythonFactory (see
 * below).  The returned pointer should be null if no conversion is possible.  If a conversion is
 * possible, the factory should pass whatever information is necessary to complete the conversion
 * (usually at least the PyPtr itself) to the derived converter's constructor.
 *
 * Derived class constructors must pass an integer penalty value to the base class constructor,
 * which indicates how appropriate the converter is for the Python object we're attempting to
 * convert.  A penalty of zero indicates a perfect match, with higher numbers indicating implicit
 * conversions (including conversions from a Python object that holds a derived classes when looking
 * for a base class reference or pointer; each level of inheritance should correspond to one unit of
 * penalty).  When converting types for overloaded functions, the maximum penalty value of all
 * arguments is used to compare different overloads to detemrine which one will be called.
 *
 * All derived classes must implement convert() to actually carry out the conversion, and provide a
 * non-trivial destructor if needed to clean up the pointer returned by convert().  A postcall()
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

typedef std::vector<std::unique_ptr<FromPythonConverter>> ConverterVector;

/*
 * Factory class for FromPythonConverter.
 *
 * FromPythonFactory instances held by a TypeRegistration for the C++ type the converters they
 * create convert to.
 */
class FromPythonFactory {
public:

    /*
     * Return a converter that maps the given Python object to a C++ object, if possible.
     *
     * If this factory cannot produce a factory that is likely to work for the given object,
     * it should return nullptr.
     *
     * The returned converter is not guaranteed to work, but it should be a likely enough
     * match that the quality of the match can be used to resolve which of several overloads
     * of a function should be called.
     */
    virtual std::unique_ptr<FromPythonConverter> apply(PyPtr const &) const = 0;

    /*
     * A unique name for this converter.
     *
     * This will be used to compare converters when they're registered to prevent duplicates (even
     * when the same templated converter is instantiated in different modules).
     */
    std::string const name;

    /*
     * Whether this converter is appropriate for lvalues.
     *
     * C++ functions that take lvalue arguments (non-const references or pointers)
     * require lvalue converters, which allow modifications to those arguments to be transferred
     * back to Python.  This can reflect either a converter that extracts a C++ object from
     * a Python object that holds it, hence truly modifying it in place, or a converter
     * that uses the postcall() method to copy temporary state back to a Python object.
     *
     * Note that some common Python types, including strings and numbers, can never
     * be converted by an lvalue converter, because they are intrinsically immutable.
     * C++ functions that use output arguments for these types must be converted to
     * return multiple values instead.
     */
    bool const is_lvalue;

    /*
     * Whether this converter only applies to pointers.
     *
     * C++ pointers can also be used to pass arrays, so converters that yield array-like
     * pointer values should set is_pointer to true so they are not used to convert to
     * non-pointer or value C++ types.
     */
    bool const is_pointer;

    /*
     * Return a copy of this converter appropriate for a different registry.
     *
     * When registries are copied, any converters that hold TypeRegistrations (in order
     * to delegate to other converters) must be copied in a way that swaps in new
     * TypeRegistrations from the new TypeRegistry.  The new TypeRegistry may not be
     * fully populated at the time a particular converter is copied, however, so
     * implementations of this method should use registry.require() to obtain any
     * new TypeRegistrations they require, without assuming anything else about those
     * registations at that time.
     */
    virtual std::unique_ptr<FromPythonFactory> clone(TypeRegistry & registry) const = 0;

    virtual ~FromPythonFactory() {}

protected:
    FromPythonFactory(std::string const & name_, bool is_lvalue_, bool is_pointer_=false) :
        name(name_), is_lvalue(is_lvalue_), is_pointer(is_pointer_)
    {}
};


template <typename T>
class FromPythonTraits {
public:
    static bool const is_lvalue = false;
    static bool const is_pointer = false;
    static TypeInfo getTypeInfo() { return makeTypeInfo<T>(); }
    static T adapt(void * converted) { return *reinterpret_cast<T*>(converted); }
};

template <typename U>
class FromPythonTraits<U const> {
public:
    static bool const is_lvalue = false;
    static bool const is_pointer = false;
    static TypeInfo getTypeInfo() { return makeTypeInfo<U>(); }
    static U const adapt(void * converted) { return *reinterpret_cast<U const *>(converted); }
};

template <typename U>
class FromPythonTraits<U *> {
public:
    static bool const is_lvalue = true;
    static bool const is_pointer = true;
    static TypeInfo getTypeInfo() { return makeTypeInfo<U>(); }
    static U * adapt(void * converted) { return reinterpret_cast<U *>(converted); }
};

template <typename U>
class FromPythonTraits<U &> {
public:
    static bool const is_lvalue = true;
    static bool const is_pointer = false;
    static TypeInfo getTypeInfo() { return makeTypeInfo<U>(); }
    static U & adapt(void * converted) { return *reinterpret_cast<U *>(converted); }
};

template <typename U>
class FromPythonTraits<U const *> {
public:
    static bool const is_lvalue = false;
    static bool const is_pointer = true;
    static TypeInfo getTypeInfo() { return makeTypeInfo<U>(); }
    static U const * adapt(void * converted) { return reinterpret_cast<U const *>(converted); }
};

template <typename U>
class FromPythonTraits<U const &> {
public:
    static bool const is_lvalue = false;
    static bool const is_pointer = false;
    static TypeInfo getTypeInfo() { return makeTypeInfo<U>(); }
    static U const & adapt(void * converted) { return *reinterpret_cast<U const *>(converted); }
};

} // namespace mcpib

#endif // !MCPIB_FromPythonConverter_hpp_INCLUDED
