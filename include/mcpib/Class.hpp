/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_Class
#define MCPIB_Class

#include "mcpib/TypeInfo.hpp"
#include "mcpib/Holder.hpp"

namespace mcpib {

class Module;


struct PyClass {

    static PyClass make(TypeInfo const & ctype);

    PyPtr makeInstance(std::unique_ptr<Holder> holder);

    PyTypeObject base;
    TypeInfo const * ctype;
};


class ClassBase {
public:

    ClassBase(ClassBase const &) = delete;
    ClassBase(ClassBase &&) = delete;

    ClassBase & operator=(ClassBase const &) = delete;
    ClassBase & operator=(ClassBase &&) = delete;

protected:

    ClassBase(std::string const & name, PyClass * py) : _name(name), _py(py) {}

private:

    friend class Module;

    PyPtr _ready(Module & module);

    std::string _name;
    PyClass * _py;
};


template <typename T>
class Class : public ClassBase {
public:

    explicit Class(std::string const & name) : ClassBase(name, getPyClass()) {}

    static PyPtr makeUniqueInstance(std::unique_ptr<T> v) {
        std::unique_ptr<Holder> holder(new UniqueHolder<T>(std::move(v)));
        return getPyClass()->makeInstance(std::move(holder));
    }

    static PyPtr makeSharedInstance(std::shared_ptr<T> v) {
        std::unique_ptr<Holder> holder(new SharedHolder<T>(std::move(v)));
        return getPyClass()->makeInstance(std::move(holder));
    }

    template <typename ...Args>
    static PyPtr makeValueInstance(Args && ...args) {
        std::unique_ptr<Holder> holder(new ValueHolder<T>(args...));
        return getPyClass()->makeInstance(std::move(holder));
    }

private:

    static PyClass * getPyClass();

};

template <typename T>
PyClass * Class<T>::getPyClass() {
    static PyClass p = PyClass::make(makeTypeInfo<T>());
    return &p;
}

} // namespace mcpib

#endif // !MCPIB_Class_hpp_INCLUDED
