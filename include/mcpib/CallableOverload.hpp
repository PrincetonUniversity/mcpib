/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_CallableOverload_hpp_INCLUDED
#define MCPIB_CallableOverload_hpp_INCLUDED

#include "mcpib/FromPython.hpp"
#include "mcpib/TypeRegistration.hpp"
#include "mcpib/metaprogramming.hpp"

#include <string>
#include <memory>

namespace mcpib {

struct ArgumentData {

    explicit ArgumentData(std::string const & name_) : name(name_) {}

    bool is_lvalue;
    std::string name;
    std::shared_ptr<TypeRegistration> registration;
};

typedef std::vector<ArgumentData> ArgumentDataVector;


class ArgumentDataBuilder {
public:

    explicit ArgumentDataBuilder(TypeRegistry & registry, std::initializer_list<std::string> names);

    template <int N, typename T>
    void apply() const {
        arguments[N].is_lvalue = AdaptFromPython<T>::is_lvalue;
        arguments[N].registration = _registry->require(makeTypeInfo<T>());
    }

    mutable ArgumentDataVector arguments;
private:
    TypeRegistry * _registry;
};


struct ArgumentConverter {

    explicit ArgumentConverter(ConverterVector const & converters) : _converters(converters) {}

    template <int N, typename T>
    T apply() const {
        return AdaptFromPython<T>::adapt(_converters[N]->convert());
    }

private:
    ConverterVector const & _converters;
};


class CallableOverloadBase;


class CallableOverloadData {
public:

    enum ErrorEnum {
        SUCCESS = 0,
        TOO_MANY,
        NO_CONVERTER
    };

    CallableOverloadData(PyPtr const & args, PyPtr const & kwds, CallableOverloadBase const * overload);

    Penalty getPenalty() const;

    PyPtr call() const;

private:
    ErrorEnum _error_state;
    int _error_position;
    std::string _error_string;
    ConverterVector _converters;
    CallableOverloadBase const * _overload;
};


class CallableOverloadBase {
public:

    explicit CallableOverloadBase(ArgumentDataVector arguments) : _arguments(std::move(arguments)) {}

    bool prep(PyPtr const & args, PyPtr const & kwds, ConverterVector & converters) const;

    virtual PyPtr call(ConverterVector const & converters) const = 0;

    virtual ~CallableOverloadBase() {}

protected:

    friend class CallableOverloadData;

    ArgumentDataVector _arguments;
};


template <typename Result, typename ...Args>
class CallableOverload : public CallableOverloadBase {
public:

    explicit CallableOverload(std::function<Result(Args...)> function, ArgumentDataVector arguments) :
        CallableOverloadBase(std::move(arguments)),
        _function(std::move(function))
    {}

    virtual PyPtr call(ConverterVector const & converters) const {
        callFunction(ArgumentConverter(converters), _function);
        // TODO: handle return value
        return PyPtr::borrow(Py_None);
    }

protected:

private:
    std::function<Result(Args...)> _function;
};


template <typename Result, typename ...Args>
std::unique_ptr<CallableOverloadBase> makeCallableOverload(
    std::function<Result(Args...)> function,
    std::initializer_list<std::string> names,
    TypeRegistry & registry
) {
    ArgumentDataBuilder builder(registry, std::move(names));
    ForEach<Args...>::apply(builder);
    return std::unique_ptr<CallableOverloadBase>(
        new CallableOverload<Result,Args...>(function, std::move(builder.arguments))
    );
}

} // namespace mcpib

#endif // !MCPIB_CallableOverload_hpp_INCLUDED
