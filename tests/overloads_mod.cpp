/*
 * Copyright (c) 2014-2015, Jim Bosch
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
initoverloads_mod(void) {
    Module module("overloads_mod", "unit tests for function overload resolution");
    module.import("mcpib.strings");
    module.import("mcpib.numbers");

    module.add(
        Callable("f0")
            .addOverload([](int i) { return "int"; }, {"i"}, module.getRegistry())
            .addOverload([](double d) { return "double"; }, {"d"}, module.getRegistry())
    );
    module.add(
        Callable("f1")
            .addOverload([](int i1, double d2) { return "id"; }, {"i1", "d2"}, module.getRegistry())
            .addOverload([](double d1, int i2) { return "di"; }, {"d1", "i2"}, module.getRegistry())
    );
}
