/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_FromPython_hpp_INCLUDED
#define MCPIB_FromPython_hpp_INCLUDED

#include "mcpib/PyPtr.hpp"
#include "mcpib/TypeRegistration.hpp"
#include "mcpib/detail/FromPythonImpl.hpp"

#include <memory>

namespace mcpib {

/*
 * High-level interface for conversion from Python to C++.
 *
 * Operations that need detailed control over exactly when different aspects of conversion should be
 * performed (e.g. argument parsing for overload resolution) should use FromPythonConverters and
 * FromPythonTraits directly, but most user code can just use FromPython.
 */
template <typename T>
class FromPython {
public:

    /*
     *  Construct a converter for the given Python object and a registry entry.
     *
     *  Users that don't have a TypeRegistration should use TypeRegistry::fromPython to
     *  create a FromPython object.
     *
     *  This constructor should not throw unless a serious system error has occurred;
     *  if a conversion is not possible, either because the TypeRegistration is null or
     *  it has no converters that apply to the given Python object, an unusable FromPython
     *  object will be returned, which can be tested by casting to bool.
     */
    FromPython(PyPtr const & p, std::shared_ptr<TypeRegistration> registration) :
        _impl(p, FromPythonTraits<T>::is_lvalue, registration)
    {}

    /*
     *  Test whether the conversion is valid, evaluating to true if it is.
     *
     *  If this evaluates to false, operator() will definitely throw PythonException.
     *  If it evaluates to true, operator() still may throw a PythonException, if the
     *  Python object appeared to be convertible at first but could not be converted
     *  in detail (for instance, if the target type is an unsigned integer and the
     *  Python value is a negative integer, OverflowError will be raised).
     */
    explicit operator bool() const { return _impl.converter; }

    /*
     *  Complete the conversion, return a C++ object.
     *
     *  If T is a reference or pointer type, the lifetime of the object returned
     *  is managed by the FromPython instance, and hence may be destroyed when
     *  the FromPython object goes out of scope.
     *
     *  If !(*this), FromPythonError will be raised (but the messages won't be very
     *  informative, so it's generally best for calling code to check in advance).
     */
    T operator()() const {
        _impl.require();
        return FromPythonTraits<T>::adapt(_impl.converter->convert());
    }

private:
    detail::FromPythonImpl _impl;
};

template <typename T>
FromPython<T> TypeRegistry::fromPython(PyPtr const & p) {
    return FromPython<T>(p, this->lookup(FromPythonTraits<T>::getTypeInfo()));
}

} // namespace mcpib

#endif // !MCPIB_FromPython_hpp_INCLUDED
