#ifndef ACTIO_VALUE_HANDLER_H
#define ACTIO_VALUE_HANDLER_H

#include <cassert>
#include <functional>

template<typename T>
struct ValueHandler {
    ValueHandler() {}

    explicit ValueHandler(T value) : value(value), prevValue(value) {}

    ValueHandler<T> &operator=(T newValue) {
        value = newValue;
        prevValue = newValue;

        return *this;
    };

    bool check() {
        changed = value != prevValue;

        if (changed) {
            prevValue = value;
        }

        return changed;
    };

    bool isChanged(bool force = false) {
        if (force) {
            return check();
        }

        return changed;
    };

    void checkAndHandle() {
        assert(handler != nullptr);

        if (check()) {
            (*handler)(value);
        }
    };

    void setHandler(std::function<void(T)> &newHandler) {
        handler = &newHandler;
    };

    T getRef() {
        return value;
    };

    T *get() {
        return &value;
    };

    void set(T newValue) {
        value = newValue;
        prevValue = newValue;
    };

private:
    T value;
    T prevValue;
    bool changed = false;
    std::function<void(T)> *handler = nullptr;
};

#endif //ACTIO_VALUE_HANDLER_H
