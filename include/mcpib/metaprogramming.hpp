/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_metaprogramming_hpp_INCLUDED
#define MCPIB_metaprogramming_hpp_INCLUDED

#include "mcpib/FromPython.hpp"
#include "mcpib/TypeRegistration.hpp"

#include <string>
#include <memory>

namespace mcpib {

namespace detail {

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
template <int N, typename ResultT, typename ...Args>
struct ArgumentTraits<N,ResultT(Args...)> {
    typedef typename std::tuple_element<N,std::tuple<Args...>>::type Type;
};

/*
 * Implementation helper for callFunction
 */
template <typename ArgumentBuilder, typename F, int ...S>
typename std::function<F>::result_type
callFunctionImpl(ArgumentBuilder builder, std::function<F> const & function, Sequence<S...>) {
    return function(builder.template apply<S,typename ArgumentTraits<S,F>::Type>()...);
}

} // namespace detail


/*
 * Call a std::function-wrapped callable, using a function object with templated apply() methods
 * to build its arguments.
 *
 * The "ArgumentBuilder" class that is used to build the arguments should look like this:
 *
 * struct ArgumentBuilder {
 *     template <int N, typename T>
 *     T apply() const {
 *         ... return the Nth argument, of type T
 *     }
 * };
 */
template <typename ArgumentBuilder, typename Result, typename ...Args>
Result callFunction(ArgumentBuilder builder, std::function<Result(Args...)> const & function) {
    return detail::callFunctionImpl(builder, function, typename detail::Generate<sizeof...(Args)>::Type());
}


} // namespace mcpib

#endif // !MCPIB_metaprogramming_hpp_INCLUDED
