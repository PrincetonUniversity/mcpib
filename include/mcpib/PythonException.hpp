/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_PythonException_hpp_INCLUDED
#define MCPIB_PythonException_hpp_INCLUDED

#include "mcpib/PyPtr.hpp"

#include <exception>
#include <string>

namespace mcpib {

class PythonException : public std::exception {
public:

    static PythonException fetch();

    static PythonException raise(PyPtr const & type, std::string message);

    std::nullptr_t restore();

    virtual char const * what() const noexcept;

    virtual ~PythonException() throw() {}

private:

    PythonException(PyPtr const & type, PyPtr const & value, PyPtr const & traceback);

    PyPtr _type;
    PyPtr _value;
    PyPtr _traceback;
    PyPtr _str;
};

} // namespace mcpib

#endif // !MCPIB_PythonException_hpp_INCLUDED
