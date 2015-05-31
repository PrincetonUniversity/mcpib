/*
 * Copyright (c) 2015, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_Module_hpp_INCLUDED
#define MCPIB_Module_hpp_INCLUDED

#include "mcpib/TypeRegistry.hpp"
#include "mcpib/Class.hpp"

#include <memory>

namespace mcpib {

class Callable;

class Module {
public:

    // Create a new mcpib Python module.
    //
    // This should only be called once per module, in that module's iniit function.
    explicit Module(char const * name, char const * doc=nullptr);

    // Create a C++ interface to an existing mcpib module.
    explicit Module(PyPtr const & py);

    // Module is not copyable.
    Module(Module const &) = delete;
    Module & operator=(Module const &) = delete;

    // Module is moveable.
    Module(Module &&) = default;
    Module & operator=(Module &&) = default;

    Module & add(Callable const & callable);

    Module & add(std::string const & name, PyPtr const & value);

    template <typename T>
    Module & add(Class<T> & cls) {
        cls._attachTo(*this, makeTypeInfo<T>());
        return *this;
    }

    TypeRegistry & getRegistry() { return _registry; }

    TypeRegistry const & getRegistry() const { return _registry; }

    // Import another mcpib module, and merge its TypeRegistry into this module's.
    //
    // All TypeRegistrations in the imported module will be copied into this module's
    // TypeRegistry, or merged with an existing TypeRegistration when one for the
    // appropriate type exists.
    // The imported module's TypeRegistry will not be affected, and TypeRegistrations are
    // never shared between modules.
    Module import(std::string const & module_name);

private:

    friend class Callable;
    friend class ClassBase;

    PyPtr _py;
    TypeRegistry _registry;
};

} // namespace mcpib

#endif // !MCPIB_Module_hpp_INCLUDED
