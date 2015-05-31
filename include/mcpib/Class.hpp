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


class ClassBase {
public:

    ClassBase(ClassBase const &) = delete;
    ClassBase(ClassBase &&) = delete;

    ClassBase & operator=(ClassBase const &) = delete;
    ClassBase & operator=(ClassBase &&) = delete;

    PyPtr makeInstance(std::unique_ptr<Holder> holder) const;

protected:

    friend class Module;

    explicit ClassBase(std::string const & name);

    PyTypeObject * _getPyTypeObject() const { return reinterpret_cast<PyTypeObject*>(_py.get()); }

    void _attachTo(Module & module, TypeInfo const & ctype);

    PyPtr _py;
};


template <typename T>
class Class : public ClassBase {
public:

    explicit Class(std::string const & name) : ClassBase(name) {}

    template <typename ...Args>
    PyPtr makeValueInstance(Args && ...args) const {
        std::unique_ptr<Holder> holder(new ValueHolder<T>(std::forward<Args>(args)...));
        return makeInstance(std::move(holder));
    }

};

} // namespace mcpib

#endif // !MCPIB_Class_hpp_INCLUDED
