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

#define ADD_ACCEPT_FUNC(name, type)                                     \
    module.add(                                                         \
        Callable("accept_" name).addOverload(                           \
            std::function<void(type)>([](type x) {}), {"x"}, module.getRegistry() \
        )                                                               \
    );                                                                  \
    module.add("bits_" name , PyPtr::steal(PyInt_FromLong(8*sizeof(type))))

PyMODINIT_FUNC
initbuiltin_numeric_mod(void) {
    Module module("builtin_numeric_mod", "unit tests for builtin numeric converters");
    module.import("mcpib.numbers");
    module.add(
        "char_is_signed",
        PyPtr::steal(PyBool_FromLong(static_cast<char>(-1) < 0))
    );
    ADD_ACCEPT_FUNC("char", char);
    ADD_ACCEPT_FUNC("signed_char", signed char);
    ADD_ACCEPT_FUNC("unsigned_char", unsigned char);
    ADD_ACCEPT_FUNC("short", short);
    ADD_ACCEPT_FUNC("unsigned_short", unsigned short);
    ADD_ACCEPT_FUNC("int", int);
    ADD_ACCEPT_FUNC("unsigned_int", unsigned int);
    ADD_ACCEPT_FUNC("long", long);
    ADD_ACCEPT_FUNC("unsigned_long", unsigned long);
    ADD_ACCEPT_FUNC("long_long", long long);
    ADD_ACCEPT_FUNC("unsigned_long_long", unsigned long long);
    ADD_ACCEPT_FUNC("float", float);
    ADD_ACCEPT_FUNC("double", double);
    ADD_ACCEPT_FUNC("long_double", long double);
}
