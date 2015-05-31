/*
 * Copyright (c) 2015, Jim Bosch
 * All rights reserved.
 *
 * mcpib is distributed under a simple BSD-like license;
 * see the LICENSE file that should be present in the root
 * of the source distribution.
 */
#ifndef MCPIB_Holder_hpp_INCLUDED
#define MCPIB_Holder_hpp_INCLUDED

#include "mcpib/PyPtr.hpp"
#include <memory>

namespace mcpib {

class Holder {
public:

    virtual void * get() = 0;

    virtual ~Holder() {}
};


template <typename T>
class ValueHolder : public Holder {
public:

    ValueHolder(ValueHolder const &) = delete;
    ValueHolder(ValueHolder &&) = default;

    ValueHolder & operator=(ValueHolder const &) = delete;
    ValueHolder & operator=(ValueHolder &&) = default;

    template <typename ...Args>
    explicit ValueHolder(Args && ... args) : _value(std::forward<Args>(args)...) {}

    virtual void * get() { return &_value; }

private:
    T _value;
};


template <typename T>
class UniqueHolder : public Holder {
public:

    UniqueHolder(UniqueHolder const &) = delete;
    UniqueHolder(UniqueHolder &&) = default;

    UniqueHolder & operator=(UniqueHolder const &) = delete;
    UniqueHolder & operator=(UniqueHolder &&) = default;

    explicit UniqueHolder(std::unique_ptr<T> p) : _ptr(std::move(p)) {}

    template <typename ...Args>
    explicit UniqueHolder(Args && ... args) :
        _ptr(new UniqueHolder(std::forward<Args>(args)...)) {}

    virtual void * get() { return _ptr.get(); }

private:
    std::unique_ptr<T> _ptr;
};


template <typename T>
class SharedHolder : public Holder {
public:

    SharedHolder(SharedHolder const &) = delete;
    SharedHolder(SharedHolder &&) = default;

    SharedHolder & operator=(SharedHolder const &) = delete;
    SharedHolder & operator=(SharedHolder &&) = default;

    explicit SharedHolder(std::shared_ptr<T> p) : _ptr(std::move(p)) {}

    template <typename ...Args>
    explicit SharedHolder(Args && ... args) :
        _ptr(std::make_shared<T>(std::forward<Args>(args)...)) {}

    virtual void * get() { return _ptr.get(); }

private:
    std::shared_ptr<T> _ptr;
};


} // namespace mcpib

#endif // !MCPIB_Holder_hpp_INCLUDED
