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

#include "mcpib/FromPythonConverter.hpp"
#include "mcpib/ToPythonConverter.hpp"
#include "mcpib/TypeInfo.hpp"

#include <vector>
#include <unordered_map>

namespace mcpib {

class TypeRegistry;

class TypeRegistration {
public:

    typedef std::vector<std::unique_ptr<FromPythonFactory>> FromPythonList;
    typedef std::unordered_map<TypeInfo,std::shared_ptr<TypeRegistration>> Map;

    /*
     * Add a new from-Python converter for this type.
     *
     * The new converter will take precedence over any existing converters for this type
     * (but older converters will be tried when newer ones fail).
     */
    void registerFromPython(std::unique_ptr<FromPythonFactory> factory);

    /*
     * Find a from-Python converter that works for the given Python object.
     *
     * Returns a null pointer if no match could be found.
     */
    std::unique_ptr<FromPythonConverter> lookupFromPython(PyPtr const & p, bool require_lvalue) const;

    // Set the converter for by-value and rvalue reference returns.
    void setMoveToPython(std::unique_ptr<MoveToPythonConverter> converter);

    // Set the converter for reference and pointer conversions to Python.
    void setRefToPython(std::unique_ptr<RefToPythonConverter> converter);

    // Set the converter for const reference and pointer conversions to Python.
    void setConstRefToPython(std::unique_ptr<ConstRefToPythonConverter> converter);

    /*
     * Convert a C++ object that matches the TypeRegistration's type to Python.
     * Convert a C++ object to Python.
     *
     * This function is defined in ToPythonTraits.hpp.
     */
    template <typename T>
    PyPtr toPython(T && v) const;

private:

    friend class TypeRegistry;

    template <typename T> friend class ToPythonTraits;

    // Copy all state into another TypeRegistration, inserting converters and creating derived-class
    // TypeRegistrations as necessary in the given registry.
    // New derived-class TypeRegistrations will only be created (via TypeRegistry::require()); they
    // will not be copied themselves.
    void _copyInto(TypeRegistration & other, TypeRegistry & registry) const;

    // Throw an appropriate exception if there is no MoveToPythonConverter.
    void _requireMoveToPython() const;

    // Throw an appropriate exception if there is no RefToPythonConverter.
    void _requireRefToPython() const;

    // Throw an appropriate exception if there is no ConstRefToPythonConverter.
    void _requireConstRefToPython() const;

    /*
     * Sequence of from-python converter factories, along with a bool indicating whether
     * the converter produces an lvalue.
     *
     * New registrations are added to the end, so we start from the end of the list and
     * iterate backwards when looking for the match, in order to try the newest ones first.
     */
    FromPythonList _from_python;

    // To-Python converter for by-value and rvalue reference returns.
    std::unique_ptr<MoveToPythonConverter> _move_to_python;

    // To-Python converter for non-const reference and pointer returns.
    std::unique_ptr<RefToPythonConverter> _ref_to_python;

    // To-Python converter for const reference and const xpointer returns.
    std::unique_ptr<ConstRefToPythonConverter> _const_ref_to_python;

    /*
     * A map of TypeRegistrations for all classes that inherit from this one.
     */
    Map _derived;
};

} // namespace mcpib

#endif // !MCPIB_TypeRegistration_hpp_INCLUDED
