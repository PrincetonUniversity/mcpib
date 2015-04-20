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
initbuiltin_strings_mod(void) {
    Module module("builtin_strings_mod", "unit tests for string converters");
    module.import("mcpib.strings");

    module.add(
        Callable("passthru_string")
            .addOverload([](std::string s) { return s; }, {"s"}, module.getRegistry())
    );
    module.add(
        Callable("passthru_cstring")
            .addOverload([](char const * s) { return s; }, {"s"}, module.getRegistry())
    );

    module.add(
        Callable("accept_char")
            .addOverload([](char s) {}, {"s"}, module.getRegistry())
    );
    module.add(
        Callable("accept_char_ptr")
            .addOverload([](char * s) {}, {"s"}, module.getRegistry())
    );
    module.add(
        Callable("accept_char_const")
            .addOverload([](char const s) {}, {"s"}, module.getRegistry())
    );
    module.add(
        Callable("accept_char_ref")
            .addOverload([](char & s) {}, {"s"}, module.getRegistry())
    );
    module.add(
        Callable("accept_char_const_ref")
            .addOverload([](char const & s) {}, {"s"}, module.getRegistry())
    );

    module.add(
        Callable("return_char")
            .addOverload([]()->char { return 25; }, {}, module.getRegistry())
    );
    module.add(
        Callable("return_char_const")
            .addOverload([]()->char const { return 25; }, {}, module.getRegistry())
    );
    module.add(
        Callable("return_char_ptr")
            .addOverload([]()->char * { static char v = 25; return &v; }, {},
                         module.getRegistry())
    );
    module.add(
        Callable("return_char_ref")
            .addOverload([]()->char & { static char v = 25; return v; }, {},
                         module.getRegistry())
    );
    module.add(
        Callable("return_char_const_ref")
            .addOverload([]()->char const & { static char v = 25; return v; }, {},
                         module.getRegistry())
    );
}
