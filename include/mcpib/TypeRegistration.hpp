/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_TypeRegistration_hpp_INCLUDED
#define MCPIB_TypeRegistration_hpp_INCLUDED

#include "mcpib/FromPython.hpp"
#include "mcpib/TypeInfo.hpp"

#include <vector>
#include <map>

namespace mcpib {

class TypeRegistration {
public:

    typedef std::vector<std::pair<FromPythonFactory,bool>> FromPythonList;
    typedef std::map<TypeInfo,std::shared_ptr<TypeRegistration>> Map;

    void registerFromPython(FromPythonFactory factory, bool is_lvalue);

    std::unique_ptr<FromPythonConverter> lookupFromPython(PyPtr const & p, bool require_lvalue) const;

private:
    FromPythonList _from_python;
    Map _derived;
};

} // namespace mcpib

#endif // !MCPIB_TypeRegistration_hpp_INCLUDED
