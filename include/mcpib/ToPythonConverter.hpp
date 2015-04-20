/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_ToPythonConverter_hpp_INCLUDED
#define MCPIB_ToPythonConverter_hpp_INCLUDED

#include "mcpib/PyPtr.hpp"

#include <memory>

namespace mcpib {

class TypeRegistry;

/*
 * Base class for to-Python conversions.
 */
class ToPythonConverter {
public:

    /*
     * Return a Python object that contains or otherwise represents the given C++ object.
     *
     * The void pointer argument is guaranteed to point to an instance of the type this
     * converter was registered with, so it should be safe to just reinterpret_cast to
     * a pointer to that type.
     */
    virtual PyPtr convert(void *) const = 0;

    // @copydoc FromPythonFactory::clone
    virtual std::unique_ptr<ToPythonConverter> clone(TypeRegistry & registry) const = 0;

    virtual ~ToPythonConverter() {}

};

} // namespace mcpib

#endif // !MCPIB_ToPythonConverter_hpp_INCLUDED
