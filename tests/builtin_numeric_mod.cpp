/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/Module.hpp"
#include "mcpib/Callable.hpp"

using namespace mcpib;

PyMODINIT_FUNC
initbuiltin_numeric_mod(void) {
    Module module("builtin_numeric_mod", "unit tests for builtin numeric converters");
    module.import("mcpib.numbers");
    module.add(
        Callable("accept_int_return_1").addOverload(
            std::function<int(int)>([](int x) { return 1; }),
            {"x"},
            module.getRegistry()
        )
    );
}
