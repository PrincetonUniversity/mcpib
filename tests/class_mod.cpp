/*
 * Copyright (c) 2014-2015, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/Module.hpp"
#include "mcpib/Class.hpp"
#include "mcpib/Callable.hpp"

using namespace mcpib;

class A {
public:

};

PyMODINIT_FUNC
initclass_mod(void) {
    Module module("class_mod", "unit tests for class wrappers");
    module.import("mcpib.strings");
    Class<A> classA("A");
    module.add(classA);
    module.add("a1", Class<A>::makeValueInstance());
    module.add(
        Callable("accept_val")
            .addOverload([](A a) { return "A"; }, {"a"}, module.getRegistry())
    );
    module.add(
        Callable("accept_ref")
            .addOverload([](A & a) { return "A"; }, {"a"}, module.getRegistry())
    );
    module.add(
        Callable("accept_cref")
            .addOverload([](A const & a) { return "A"; }, {"a"}, module.getRegistry())
    );
    module.add(
        Callable("accept_ptr")
            .addOverload([](A * a) { return "A"; }, {"a"}, module.getRegistry())
    );
    module.add(
        Callable("accept_cptr")
            .addOverload([](A const * a) { return "A"; }, {"a"}, module.getRegistry())
    );
}

template class mcpib::Class<A>;
