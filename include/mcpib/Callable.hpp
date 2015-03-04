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

    template <typename Result, typename ...Args>
    void addOverload(
        std::function<Result(Args...)> function,
        std::initializer_list<std::string> names,
        TypeRegistry & registry
    ) {
        _overloads.push_back(makeCallableOverload(std::move(function), std::move(names), registry));
    }

    PyPtr call(PyPtr const & args, PyPtr const & kwds) const;

private:
    std::vector<std::unique_ptr<CallableOverloadBase>> _overloads;
};

} // namespace mcpib

#endif // !MCPIB_Callable_hpp_INCLUDED
