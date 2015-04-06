/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_DETAIL_FromPythonImpl_hpp_INCLUDED
#define MCPIB_DETAIL_FromPythonImpl_hpp_INCLUDED

#include "mcpib/PyPtr.hpp"
#include "mcpib/TypeRegistration.hpp"
#include "mcpib/FromPythonConverter.hpp"

#include <memory>

namespace mcpib { namespace detail {

class FromPythonImpl {
public:

    FromPythonImpl(PyPtr const & p, bool is_lvalue, std::shared_ptr<TypeRegistration> registration);

    void require();

    ~FromPythonImpl();

    std::unique_ptr<FromPythonConverter> converter;
};

}} // namespace mcpib::detail

#endif // !MCPIB_DETAIL_FromPythonImpl_hpp_INCLUDED
