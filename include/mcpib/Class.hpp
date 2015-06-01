/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_Class_hpp_INCLUDED
#define MCPIB_Class_hpp_INCLUDED

#include "mcpib/TypeInfo.hpp"
#include "mcpib/Holder.hpp"
#include "mcpib/TypeRegistration.hpp"
#include "mcpib/detail/InstanceFromPython.hpp"

namespace mcpib {

class Module;


class ClassBase {
public:

    ClassBase(ClassBase const &) = delete;
    ClassBase(ClassBase &&) = delete;

    ClassBase & operator=(ClassBase const &) = delete;
    ClassBase & operator=(ClassBase &&) = delete;

    PyPtr getPyObject() const { return _py; }

    virtual ~ClassBase() {}

protected:

    friend class Module;

    explicit ClassBase(std::string const & name, std::initializer_list<PyPtr> bases);

    PyPtr makeInstance(std::unique_ptr<HolderBase> holder) const;

    virtual void _registerConverters(std::string const & name, TypeRegistration & registration) const = 0;

    PyTypeObject * _getPyTypeObject() const { return reinterpret_cast<PyTypeObject*>(_py.get()); }

    void _attachTo(Module & module, TypeInfo const & ctype);

    PyPtr _py;
};


template <typename T>
class Class : public ClassBase {
public:

    explicit Class(std::string const & name) : ClassBase(name, {}) {}

    explicit Class(std::string const & name, ClassBase const & base) :
        ClassBase(name, {base.getPyObject()}) {}

    explicit Class(std::string const & name, std::initializer_list<PyPtr> bases) :
        ClassBase(name, bases) {}

    template <typename ...Args>
    PyPtr makeValueInstance(Args && ...args) const {
        std::unique_ptr<HolderBase> holder(new ValueHolder<T>(std::forward<Args>(args)...));
        return makeInstance(std::move(holder));
    }

protected:

    virtual void _registerConverters(std::string const & name, TypeRegistration & registration) const {
        std::unique_ptr<FromPythonFactory> fromPython(
            new detail::InstanceFromPythonFactory<T>(_py, name)
        );
        registration.registerFromPython(std::move(fromPython));
    }

};

} // namespace mcpib

#endif // !MCPIB_Class_hpp_INCLUDED
