/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_TypeRegistry_hpp_INCLUDED
#define MCPIB_TypeRegistry_hpp_INCLUDED

#include "mcpib/TypeRegistration.hpp"

namespace mcpib {

template <typename T> class FromPython;

class TypeRegistry {
public:

    // TypeRegistry is not copyable.
    TypeRegistry(TypeRegistry const &) = delete;
    TypeRegistry & operator=(TypeRegistry const &) = delete;

    // TypeRegistry is moveable.
    TypeRegistry(TypeRegistry &&) = default;
    TypeRegistry & operator=(TypeRegistry &&) = default;

    /*
     * Find the registration for the given type, returning a null pointer if not found.
     */
    std::shared_ptr<TypeRegistration> lookup(TypeInfo const & t) const;

    /*
     * Find the registration for the given type, inserting a new one if not found.
     */
    std::shared_ptr<TypeRegistration> require(TypeInfo const & t);

    /*
     * Return a from-Python converter that maps the given Python object to the given C++ type.
     *
     * This function is defined in FromPython.hpp.
     */
    template <typename T>
    FromPython<T> fromPython(PyPtr const & p) const;

    /*
     * Convert a C++ object to Python.
     *
     * This function is defined in ToPythonTraits.hpp.
     */
    template <typename T>
    PyPtr toPython(T && v) const;

private:

    friend class Module;

    static PyPtr _make();

    // Find the registration for given type, throwing ToPythonError if not found.
    std::shared_ptr<TypeRegistration> _lookupToPython(TypeInfo const & t) const;

    TypeRegistry() {}

    void _checkPyType();

    void _import(TypeRegistry const & other);

    PyPtr _py;
};

} // namespace mcpib

#endif // !MCPIB_TypeRegistration_hpp_INCLUDED
