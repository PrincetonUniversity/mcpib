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

#include "mcpib/FromPythonConverter.hpp"
#include "mcpib/TypeRegistry.hpp"
#include "mcpib/metaprogramming.hpp"
#include "mcpib/ToPythonTraits.hpp"
#include "mcpib/GetSignature.hpp"

#include <string>
#include <memory>

namespace mcpib {

struct ArgumentData {

    explicit ArgumentData(std::string name_) : name(std::move(name_)) {}

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
        arguments[N].is_lvalue = FromPythonTraits<T>::is_lvalue;
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
        return FromPythonTraits<T>::adapt(_converters[N]->convert());
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
        NO_CONVERTER,
        UNKNOWN_KWARG,
        DUPLICATE_ARG
    };

    CallableOverloadData(PyPtr const & args, PyPtr const & kwds, CallableOverloadBase const * overload);

    CallableOverloadData(CallableOverloadData const &) = delete;
    CallableOverloadData & operator=(CallableOverloadData const &) = delete;

    CallableOverloadData(CallableOverloadData &&) = default;
    CallableOverloadData & operator=(CallableOverloadData &&) = default;

    ErrorEnum getState() const { return _error_state; }

    PyPtr raiseException(std::string const & function_name) const;

    Penalty getPenalty() const;

    PyPtr call() const;

private:
    ErrorEnum _error_state;
    int _error_position;
    std::string _error_string;
    ConverterVector _converters;
    CallableOverloadBase const * _overload;
};


template <typename Result>
class ReturnConverter {
public:

    explicit ReturnConverter(TypeRegistry & registry) :
        _registration(registry.require(ToPythonTraits<Result>::getTypeInfo()))
    {}

    template <typename ArgumentBuilder, typename ...Args>
    PyPtr apply(ArgumentBuilder builder, std::function<Result(Args...)> const & function) const {
        return _registration->toPython(callFunction(builder, function));
    }

private:
    std::shared_ptr<TypeRegistration> _registration;
};


template <>
class ReturnConverter<void> {
public:

    explicit ReturnConverter(TypeRegistry &) {}

    template <typename ArgumentBuilder, typename ...Args>
    PyPtr apply(ArgumentBuilder builder, std::function<void(Args...)> const & function) const {
        callFunction(builder, function);
        return PyPtr::borrow(Py_None);
    }

};


class CallableOverloadBase {
public:

    explicit CallableOverloadBase(ArgumentDataVector arguments) : _arguments(std::move(arguments)) {}

    virtual ~CallableOverloadBase() {}

protected:

    friend class CallableOverloadData;

    virtual PyPtr call(ConverterVector const & converters) const = 0;

    ArgumentDataVector _arguments;
};


template <typename Result, typename ...Args>
class CallableOverload : public CallableOverloadBase {
public:

    explicit CallableOverload(
        std::function<Result(Args...)> function,
        ArgumentDataVector arguments,
        TypeRegistry & registry
    ) :
        CallableOverloadBase(std::move(arguments)),
        _function(std::move(function)),
        _return_converter(registry)
    {}

protected:

    virtual PyPtr call(ConverterVector const & converters) const {
        return _return_converter.apply(ArgumentConverter(converters), _function);
    }

private:
    std::function<Result(Args...)> _function;
    ReturnConverter<Result> _return_converter;
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
        new CallableOverload<Result,Args...>(std::move(function), std::move(builder.arguments), registry)
    );
}

template <typename F>
std::unique_ptr<CallableOverloadBase> makeCallableOverload(
    F && function,
    std::initializer_list<std::string> names,
    TypeRegistry & registry
) {
    return makeCallableOverload(
        std::function<GetSignature<F>>(std::forward<F>(function)),
        std::move(names),
        registry
    );
}

} // namespace mcpib

#endif // !MCPIB_CallableOverload_hpp_INCLUDED
