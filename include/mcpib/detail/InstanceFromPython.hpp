/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/Class.hpp"
#include "mcpib/Holder.hpp"
#include "mcpib/FromPythonConverter.hpp"

#include <limits>

namespace mcpib { namespace detail {


HolderBase * getHolder(PyPtr const & p);


template <typename Native, typename Target=Native>
class InstanceFromPythonConverter : public FromPythonConverter {
public:

    explicit InstanceFromPythonConverter(PyPtr const & p) : FromPythonConverter(0), _p(p) {}

    virtual void * convert() {
        return static_cast<Target*>(static_cast<Holder<Native>*>(getHolder(_p))->get());
    }

private:
    PyPtr _p;
};


template <typename Native, typename Target=Native>
class InstanceFromPythonFactory: public FromPythonFactory {
public:

    explicit InstanceFromPythonFactory(PyPtr const & cls, std::string const & name) :
        FromPythonFactory(name + ".direct", true, false),
        _cls(cls)
    {}

    virtual std::unique_ptr<FromPythonConverter> apply(PyPtr const & p) const {
        if (Py_TYPE(p.get()) == reinterpret_cast<PyTypeObject*>(_cls.get()))  {
            return std::unique_ptr<FromPythonConverter>(new InstanceFromPythonConverter<Native,Target>(p));
        }
        return nullptr;
    }

    virtual std::unique_ptr<FromPythonFactory> clone(TypeRegistry & registry) const {
        return std::unique_ptr<FromPythonFactory>(new InstanceFromPythonFactory(_cls, name));
    }

private:
    PyPtr _cls;
};


}} // namespace mcpib::detail
