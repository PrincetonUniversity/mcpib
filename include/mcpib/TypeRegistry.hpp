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

class TypeRegistry {
public:

    TypeRegistry(TypeRegistry const &) = delete;

    TypeRegistry(TypeRegistry &&) = delete;

    TypeRegistry & operator=(TypeRegistry const &) = delete;

    TypeRegistry & operator=(TypeRegistry &&) = delete;

    /*
     * Find the registration for the given type, returning a null pointer if not found.
     */
    std::shared_ptr<TypeRegistration> lookup(TypeInfo const & t) const;

    /*
     * Find the registration for the given type, inserting a new one if not found.
     */
    std::shared_ptr<TypeRegistration> require(TypeInfo const & t);

private:

    friend class Module;

    static PyPtr _make();

    TypeRegistry() {}

    void _checkPyType();

    PyPtr _py;
};

} // namespace mcpib

#endif // !MCPIB_TypeRegistration_hpp_INCLUDED
