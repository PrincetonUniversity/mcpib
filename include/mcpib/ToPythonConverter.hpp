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
 * Base class for to-Python conversions that move or copy C++ objects.
 */
class MoveToPythonConverter {
public:

    /*
     * Return a Python object that contains or otherwise represents the given C++ object.
     *
     * The void pointer argument is guaranteed to point to an instance of the type this
     * converter was registered with, so it should be safe to just reinterpret_cast to
     * a pointer to that type.  That pointer should not be held directly by the returned
     * Python object, however, as it will likely go of scope immediately after this call.
     * Instead, implementations that return a held C++ object should move-construct a
     * new instance from the dereferenced pointer.
     */
    virtual PyPtr convert(void *) const = 0;

    // @copydoc FromPythonFactory::clone
    virtual std::unique_ptr<MoveToPythonConverter> clone(TypeRegistry & registry) const = 0;

    virtual ~MoveToPythonConverter() {}

};


/*
 * Base class for to-Python conversions that put non-const C++ pointers or references in Python objects.
 */
class RefToPythonConverter {
public:

    /*
     * Return a Python object that contains or otherwise represents the given C++ object.
     *
     * The void pointer argument is guaranteed to point to an instance of the type this
     * converter was registered with, so it should be safe to just reinterpret_cast to
     * a pointer to that type.  That pointer should be held directly by the returned
     * Python object.
     */
    virtual PyPtr convert(void *) const = 0;

    // @copydoc FromPythonFactory::clone
    virtual std::unique_ptr<RefToPythonConverter> clone(TypeRegistry & registry) const = 0;

    virtual ~RefToPythonConverter() {}

};


/*
 * Base class for to-Python conversions that put const C++ pointers or references in Python objects.
 */
class ConstRefToPythonConverter {
public:

    /*
     * Return a Python object that contains or otherwise represents the given C++ object.
     *
     * The void pointer argument is guaranteed to point to an instance of the type this
     * converter was registered with, so it should be safe to just reinterpret_cast to
     * a pointer to that type.  That pointer should be held directly by the returned
     * Python object.
     */
    virtual PyPtr convert(void const *) const = 0;

    // @copydoc FromPythonFactory::clone
    virtual std::unique_ptr<ConstRefToPythonConverter> clone(TypeRegistry & registry) const = 0;

    virtual ~ConstRefToPythonConverter() {}

};

} // namespace mcpib

#endif // !MCPIB_ToPythonConverter_hpp_INCLUDED
