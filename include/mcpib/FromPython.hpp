/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_FromPython_hpp_INCLUDED
#define MCPIB_FromPython_hpp_INCLUDED

#include "mcpib/PyPtr.hpp"

#include <memory>
#include <vector>

namespace mcpib {

class FromPythonConverter {
public:

    explicit FromPythonConverter(int penalty_) : penalty(penalty_) {}

    int const penalty;

    virtual void * convert() = 0;

    virtual void postcall() {}

    virtual ~FromPythonConverter() {}

};

typedef std::unique_ptr<FromPythonConverter> (*FromPythonFactory)(PyPtr const &);

typedef std::vector<std::unique_ptr<FromPythonConverter>> ConverterVector;

} // namespace mcpib

#endif // !MCPIB_FromPython_hpp_INCLUDED
