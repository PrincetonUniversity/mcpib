/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/detail/FromPythonImpl.hpp"
#include "mcpib/WrapperError.hpp"

namespace mcpib { namespace detail {

FromPythonImpl::FromPythonImpl(
    PyPtr const & p,
    bool is_lvalue,
    bool is_pointer,
    std::shared_ptr<TypeRegistration> registration
) :
    converter(registration ? registration->lookupFromPython(p, is_lvalue, is_pointer) : nullptr)
{}

void FromPythonImpl::require() {
    if (!converter) {
        throw raiseFromPythonError("No matching converter found.");
    }
}

FromPythonImpl::~FromPythonImpl() {
    converter->postcall();
}

}} // namespace mcpib::detail
