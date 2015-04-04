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

#include <memory>

namespace mcpib {

class Callable;

class Module {
public:

    explicit Module(char const * name, char const * doc=nullptr);

    explicit Module(PyPtr const & py);

    Module(Module const &) = delete;

    Module(Module &&) = delete;

    Module & operator=(Module const &) = delete;

    Module & operator=(Module &&) = delete;

    Module & add(Callable const & callable);

    TypeRegistry & getRegistry() { return _registry; }

    TypeRegistry const & getRegistry() const { return _registry; }

private:

    friend class Callable;

    PyPtr _py;
    TypeRegistry _registry;
};

} // namespace mcpib

#endif // !MCPIB_Module_hpp_INCLUDED
