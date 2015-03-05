/*
 * Copyright (c) 2014, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */

#include "mcpib/Callable.hpp"
#include "mcpib/PythonException.hpp"

#include <limits>

namespace mcpib {

PyPtr Callable::call(PyPtr const & args, PyPtr const & kwds) const {
    try {
        std::vector<CallableOverloadData> data;
        data.reserve(_overloads.size());
        std::size_t best_index = -1;
        Penalty best_penalty = std::numeric_limits<unsigned int>::max();
        for (std::size_t i = 0; i < _overloads.size(); ++i) {
            data.emplace_back(args, kwds, _overloads[i].get());
            if (data[i].getState() == CallableOverloadData::SUCCESS) {
                Penalty new_penalty = data[i].getPenalty();
                if (new_penalty < best_penalty) {
                    best_index = i;
                    best_penalty = new_penalty;
                    if (best_penalty == 0) break;
                }
            }
        }
        if (best_index < 0) {
            // no matching overload; need to raise an exception
        }
        return data[best_index].call();
    } catch (PythonException & err) {
        return err.restore();
    } catch (std::exception & err) {
        std::string message("Unexpected C++ exception: ");
        message += std::string(err.what());
        PyErr_SetString(PyExc_SystemError, message.c_str());
        return nullptr;
    } catch (...) {
        PyErr_SetString(PyExc_SystemError, "Unexpected non-exception C++ throw.");
        return nullptr;
    }
}

} // namespace mcpib
