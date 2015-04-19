/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/Module.hpp"
#include "mcpib/FromPythonConverter.hpp"
#include "mcpib/ToPythonConverter.hpp"
#include "mcpib/PythonException.hpp"

#include <climits>

namespace mcpib {
namespace {

template <typename Converter>
class StringFromPythonFactory : public FromPythonFactory {
public:

    StringFromPythonFactory(std::string const & name, bool is_pointer) :
        FromPythonFactory(name, false, is_pointer)
    {}

    virtual std::unique_ptr<FromPythonConverter> apply(PyPtr const & p) const {
        if (PyString_Check(p.get())) {
            return std::unique_ptr<FromPythonConverter>(new Converter(p));
        }
        return nullptr;
    }

    virtual std::unique_ptr<FromPythonFactory> clone(TypeRegistry & registry) const {
        return std::unique_ptr<FromPythonFactory>(new StringFromPythonFactory(this->name, this->is_pointer));
    };

};

class StringFromPythonConverter : public FromPythonConverter {
public:

    StringFromPythonConverter(PyPtr const & p) : FromPythonConverter(0), _p(p) {}

    virtual void * convert() {
        _value = PyString_AS_STRING(_p.get());
        return &_value;
    }

private:
    PyPtr _p;
    std::string _value;
};

class CStringFromPythonConverter : public FromPythonConverter {
public:

    CStringFromPythonConverter(PyPtr const & p) : FromPythonConverter(0), _p(p) {}

    virtual void * convert() {
        return PyString_AS_STRING(_p.get());
    }

private:
    PyPtr _p;
};

class StringToPythonConverter : public MoveToPythonConverter {
public:

    virtual PyPtr convert(void * v) const {
        std::string const * s = reinterpret_cast<std::string*>(v);
        return PyPtr::steal(PyString_FromStringAndSize(s->data(), s->size()));
    }

    virtual std::unique_ptr<MoveToPythonConverter> clone(TypeRegistry & registry) const {
        return std::unique_ptr<MoveToPythonConverter>(new StringToPythonConverter());
    }

};

class CStringToPythonConverter : public MoveToPythonConverter {
public:

    virtual PyPtr convert(void * v) const {
        char const * s = reinterpret_cast<char*>(v);
        return PyPtr::steal(PyString_FromString(s));
    }

    virtual std::unique_ptr<MoveToPythonConverter> clone(TypeRegistry & registry) const {
        return std::unique_ptr<MoveToPythonConverter>(new CStringToPythonConverter());
    }

};

} // anonymous
} // namespace mcpib

using namespace mcpib;

PyMODINIT_FUNC
initstrings(void) {
    Module module("strings");
    TypeRegistry & registry = module.getRegistry();

    std::shared_ptr<Registration> string_reg = registry.require(makeTypeInfo<std::string>());
    string_reg->registerFromPython(
        std::unique_ptr<FromPythonFactory>(
            new StringFromPythonFactory<StringFromPythonConverter>("mcpib.strings.string", false)
        )
    );
    string_reg->setMoveToPython(
        std::unique_ptr<MoveToPythonConverter>(new StringToPythonConverter())
    );

    std::shared_ptr<Registration> cstring_reg = registry.require(makeTypeInfo<char>());
    cstring_reg->registerFromPython(
        std::unique_ptr<FromPythonFactory>(
            new StringFromPythonFactory<CStringFromPythonConverter>("mcpib.strings.cstring", true)
        )
    );
    cstring_reg->setConstRefToPython(
        std::unique_ptr<MoveToPythonConverter>(new CStringToPythonConverter())
    );

}
