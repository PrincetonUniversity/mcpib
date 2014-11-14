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
#include <unordered_map>

namespace mcpib {

class TypeRegistration {
public:

    typedef std::vector<std::pair<FromPythonFactory,bool>> FromPythonList;
    typedef std::unordered_map<TypeInfo,std::shared_ptr<TypeRegistration>> Map;

    /*
     * Add a new from-Python converter for this type.
     *
     * The new converter will take precedence over any existing converters for this type
     * (but older converters will be tried when newer ones fail).
     */
    void registerFromPython(FromPythonFactory factory, bool is_lvalue);

    /*
     * Find a from-Python converter that works for the given Python object.
     *
     * Returns a null pointer if no match could be found.
     */
    std::unique_ptr<FromPythonConverter> lookupFromPython(PyPtr const & p, bool require_lvalue) const;

private:

    /*
     * Sequence of from-python converter factories, along with a bool indicating whether
     * the converter produces an lvalue.
     *
     * New registrations are added to the end, so we start from the end of the list and
     * iterate backwards when looking for the match, in order to try the newest ones first.
     */
    FromPythonList _from_python;

    /*
     * A map of TypeRegistrations for all classes that inherit from this one.
     *
     * Note: I don't remember why we need this; seems like a list of base classes would be more useful...
     */
    Map _derived;
};


class TypeRegistry {
public:

    /*
     * Find the registration for the given type, returning a null pointer if not found.
     */
    std::shared_ptr<TypeRegistration> lookup(TypeInfo const & t) const;

    /*
     * Find the registration for the given type, inserting a new one if not found.
     */
    std::shared_ptr<TypeRegistration> require(TypeInfo const & t);

private:
    TypeRegistration::Map _map;
};

} // namespace mcpib

#endif // !MCPIB_TypeRegistration_hpp_INCLUDED
