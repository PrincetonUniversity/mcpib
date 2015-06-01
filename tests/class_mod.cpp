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

class A {};

class B : public A {};

class C {
public:
    virtual ~C() {}
};

class D : public virtual C {};

class E : public virtual C {};

class F : public D, public E {};

PyMODINIT_FUNC
initclass_mod(void) {
    Module module("class_mod", "unit tests for class wrappers");
    module.import("mcpib.strings");

    Class<A> classA("A");
    module.add(classA);
    module.add("a1", classA.makeValueInstance());
    Class<B> classB("B", classA);
    module.add(classB);
    module.add("b1", classB.makeValueInstance());

    Class<C> classC("C");
    module.add(classC);
    module.add("c1", classC.makeValueInstance());
    Class<D> classD("D", classC);
    module.add(classD);
    module.add("d1", classD.makeValueInstance());
    Class<E> classE("E", classC);
    module.add(classE);
    module.add("e1", classE.makeValueInstance());
    Class<F> classF("F", {classD.getPyObject(), classE.getPyObject()});
    module.add(classF);
    module.add("f1", classF.makeValueInstance());

    module.add(
        Callable("accept_val")
            .addOverload([](A a) { return "A"; }, {"a"}, module.getRegistry())
            .addOverload([](B b) { return "B"; }, {"b"}, module.getRegistry())
    );
    module.add(
        Callable("accept_ref")
            .addOverload([](A & a) { return "A"; }, {"a"}, module.getRegistry())
            .addOverload([](B & b) { return "B"; }, {"b"}, module.getRegistry())
    );
    module.add(
        Callable("accept_cref")
            .addOverload([](A const & a) { return "A"; }, {"a"}, module.getRegistry())
            .addOverload([](B const & b) { return "B"; }, {"b"}, module.getRegistry())
    );
    module.add(
        Callable("accept_ptr")
            .addOverload([](A * a) { return "A"; }, {"a"}, module.getRegistry())
            .addOverload([](B * b) { return "B"; }, {"b"}, module.getRegistry())
    );
    module.add(
        Callable("accept_cptr")
            .addOverload([](A const * a) { return "A"; }, {"a"}, module.getRegistry())
            .addOverload([](B const * b) { return "B"; }, {"b"}, module.getRegistry())
    );
}

template class mcpib::Class<A>;
