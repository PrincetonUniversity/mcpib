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

#include <string>
#include <memory>

namespace mcpib {

class ArgumentData {
public:
    bool is_lvalue;
    std::string name;
    std::shared_ptr<TypeRegistration> registration;
};

/*
 *  The Generate class defines a Sequence of integers, from zero to one less than its template argument;
 *  that is,
 *
 *  Generate<2>::Type -> Sequence<0,1>
 *  Generate<3>::Type -> Sequence<0,1,2>
 */
template<int ...> struct Sequence {};
template<int N, int ...S> struct Generate : Generate<N-1, N-1, S...> {};
template<int ...S> struct Generate<0, S...>{ typedef Sequence<S...> Type; };


/*
 * A traits class to extract the type of the Nth argument of a function, as defined by
 * the template parameter F as in e.g. std::function<F>.
 */
template <int N, typename F> struct ArgumentTraits;

template <int N, typename Result, typename ...Args>
struct ArgumentTraits<N,Result(Args...)> {
    typedef typename std::tuple_element<N,std::tuple<Args...>>::type Type;
};


template <int N, typename F>
typename ArgumentTraits<N,F>::Type convertArg(
    std::function<F> const & function,
    ConverterVector & converters
) {
    return AdaptFromPython<typename ArgumentTraits<N,F>::Type>(converters[N]->convert());
}

class CallableOverloadBase {
public:

    explicit CallableOverloadBase(std::vector<ArgumentData> arguments) : _arguments(std::move(arguments)) {}

    PyPtr call(PyPtr const & args, PyPtr const & kwds) const;

    virtual ~CallableOverloadBase() {}

protected:

    virtual PyPtr _call(ConverterVector converters) const = 0;

    std::vector<ArgumentData> _arguments;
};

template <typename Result, typename ...Args>
class CallableOverload : public CallableOverloadBase {
public:

    explicit CallableOverload(std::function<Result(Args...)> function, std::vector<ArgumentData> arguments) :
        CallableOverloadBase(std::move(arguments)),
        _function(std::move(function))
    {}

protected:

    template <int ...S>
    PyPtr _callImpl(ConverterVector converters, Sequence<S...>) const {
        _function(convertArg<S>(_function, converters)...);
        // TODO: handle return value
        return PyPtr::borrow(Py_None);
    }

    virtual PyPtr _call(ConverterVector converters) const {
        _callImpl(std::move(converters), typename Generate<sizeof...(Args)>::Type());
    }

private:
    std::function<Result(Args...)> _function;
};

} // namespace mcpib

#endif // !MCPIB_CallableOverload_hpp_INCLUDED
