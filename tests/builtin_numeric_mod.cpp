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

#define ADD_FUNCS(name, type)                                           \
    module.add(                                                         \
        Callable("passthru_" name).addOverload(                         \
            [](type x) { return x; }, {"x"}, module.getRegistry()       \
        )                                                               \
    );                                                                  \
    module.add("bits_" name , PyPtr::steal(PyInt_FromLong(8*sizeof(type))))

PyMODINIT_FUNC
initbuiltin_numeric_mod(void) {
    Module module("builtin_numeric_mod", "unit tests for builtin numeric converters");
    module.import("mcpib.numbers");
    ADD_FUNCS("signed_char", signed char);
    ADD_FUNCS("unsigned_char", unsigned char);
    ADD_FUNCS("short", short);
    ADD_FUNCS("unsigned_short", unsigned short);
    ADD_FUNCS("int", int);
    ADD_FUNCS("unsigned_int", unsigned int);
    ADD_FUNCS("long", long);
    ADD_FUNCS("unsigned_long", unsigned long);
    ADD_FUNCS("long_long", long long);
    ADD_FUNCS("unsigned_long_long", unsigned long long);
    ADD_FUNCS("float", float);
    ADD_FUNCS("double", double);
    ADD_FUNCS("long_double", long double);
}
