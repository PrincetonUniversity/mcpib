/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_type_id_hpp_INCLUDED
#define MCPIB_type_id_hpp_INCLUDED

#include "mcpib/PyPtr.hpp"

#include <typeinfo>
#include <cstring>
#include <string>

namespace mcpib {

// Idea copied from Boost.Python v2 implementation;
// this provides a copyable TypeInfo class that works
// across shared library boundaries.

#if (defined(__GNUC__) && __GNUC__ >= 3)       \
 || defined(_AIX) \
 || (   defined(__sgi) && defined(__host_mips)) \
 || (defined(__hpux) && defined(__HP_aCC)) \
 || (defined(linux) && defined(__INTEL_COMPILER) && defined(__ICC))
#define MCPIB_TYPE_ID_NAME
#endif

struct TypeInfo {

    TypeInfo(std::type_info const & t=typeid(void)) : _base_type(
#ifdef MCPIB_TYPE_ID_NAME
        t.name()
#else
        &t
#endif
    ) {}

    bool operator==(TypeInfo const & other) const {
#ifdef MCPIB_TYPE_ID_NAME
        return !std::strcmp(_base_type, other._base_type);
#else
        return _base_type == other._base_type;
#endif
    }

    bool operator<(TypeInfo const & other) const {
        return std::strcmp(name(), other.name()) < 0;
    }

    char const * name() const {
        return _base_type
#ifndef MCPIB_TYPE_ID_NAME
            ->name()
#endif
            ;
    }

    std::string demangle() const;

 private:
#ifdef MCPIB_TYPE_ID_NAME
    typedef char const* BaseID;
#else
    typedef std::type_info const* BaseID;
#endif
    BaseID _base_type;
};

template <typename T>
inline TypeInfo makeTypeInfo() { return TypeInfo(typeid(T)); }

} // namespace mcpib

namespace std {

template <>
struct hash<mcpib::TypeInfo> {
    typedef mcpib::TypeInfo argument_type;
    typedef std::size_t result_type;

    result_type operator()(argument_type const & t) const {
        return hash<std::string>()(t.name());
    }
};

} // namespace std

#endif // !MCPIB_type_id_hpp_INCLUDED
