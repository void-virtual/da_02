#pragma once

template <typename T>
class TOptional {

public:
    TOptional() = default;

    explicit TOptional(const T& newValue) : value(newValue), hasValue(true) {}

    const T& operator* () const {
        if (hasValue) {
            return value;
        }
        throw std::runtime_error("this is kinda segfault");
    }

    T& operator* () {
        if (hasValue) {
            return value;
        }
        throw std::runtime_error("this is kinda segfault");
    }

    operator bool() const {
        return hasValue;
    }

private:
    T value;
    bool hasValue = false;
};