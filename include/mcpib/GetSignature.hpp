/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_GetSignature_hpp_INCLUDED
#define MCPIB_GetSignature_hpp_INCLUDED

#include "mcpib/PyPtr.hpp"  // only needed because Python #include needs to come before std library

namespace mcpib {
namespace detail {

template <typename T> struct RemoveClass {};

template <typename C, typename R, typename ...A>
struct RemoveClass<R(C::*)(A...)> {
    using Type = R(A...);
};

template <typename C, typename R, typename ...A>
struct RemoveClass<R(C::*)(A...) const> { using Type = R(A...); };

template <typename C, typename R, typename ...A>
struct RemoveClass<R(C::*)(A...) volatile> { using Type = R(A...); };

template <typename C, typename R, typename ...A>
struct RemoveClass<R(C::*)(A...) const volatile> { using Type = R(A...); };

template <typename T>
struct GetSignatureImpl {
    using Type = typename RemoveClass<decltype(&std::remove_reference<T>::type::operator())>::Type;
};

template <typename R, typename... A>
struct GetSignatureImpl<R(A...)> {
    using Type = R(A...);
};

template <typename R, typename... A>
struct GetSignatureImpl<R(&)(A...)> {
    using Type = R(A...);
};

template <typename R, typename... A>
struct GetSignatureImpl<R(*)(A...)> {
    using Type = R(A...);
};

} // namespace detail

template <typename T>
using GetSignature = typename detail::GetSignatureImpl<T>::Type;

} // namespace mcpib

#endif // !MCPIB_GetSignature_hpp_INCLUDED
