/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_Callable_hpp_INCLUDED
#define MCPIB_Callable_hpp_INCLUDED

#include "mcpib/CallableOverload.hpp"

namespace mcpib {

class Callable {
public:

    /*
     * Construct a new empty Callable.
     */
    explicit Callable(std::string name);

    /*
     * Construct from an existing Python Callable object.
     */
    explicit Callable(PyPtr const & py);

    template <typename F>
    Callable & addOverload(
        F && function,
        std::initializer_list<std::string> names,
        TypeRegistry & registry
    ) {
        _addOverload(makeCallableOverload(std::forward<F>(function), std::move(names), registry));
        return *this;
    }

    PyPtr call(PyPtr const & args, PyPtr const & kwds) const;

    std::string const & getName() const;

private:

    friend class Module;

    void _addOverload(std::unique_ptr<CallableOverloadBase> && overload);

    PyPtr _py;
};

} // namespace mcpib

#endif // !MCPIB_Callable_hpp_INCLUDED
