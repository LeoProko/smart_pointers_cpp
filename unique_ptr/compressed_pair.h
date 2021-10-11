// Leo Proko
// leoprokowork@gmail.com

#pragma once

#include <type_traits>
#include <utility>

template <class T, int tag, bool ebo = std::is_empty_v<T> && !std::is_final_v<T>>
class CompressedPairElement {
private:
    T value_;

public:
    CompressedPairElement() = default;

    template <typename P>
    CompressedPairElement(P&& value) : value_(std::forward<P>(value)) {
    }

    T& Get() {
        return value_;
    }

    const T& Get() const {
        return value_;
    }
};

template <class T, int tag>
class CompressedPairElement<T, tag, true> : public T {
public:
    CompressedPairElement() = default;

    template <typename P>
    CompressedPairElement(P&&) {
    }

    T& Get() {
        return *this;
    }

    const T& Get() const {
        return *this;
    }
};

template <typename F, typename S>
class CompressedPair : public CompressedPairElement<F, 0>, public CompressedPairElement<S, 1> {
private:
    using First = CompressedPairElement<F, 0>;
    using Second = CompressedPairElement<S, 1>;

public:
    CompressedPair() : First(F()), Second(S()) {
    }

    template <typename T, typename U>
    CompressedPair(T&& first, U&& second)
        : First(std::forward<T>(first)), Second(std::forward<U>(second)) {
    }

    F& GetFirst() {
        return First::Get();
    }

    const F& GetFirst() const {
        return First::Get();
    }

    S& GetSecond() {
        return Second::Get();
    };

    const S& GetSecond() const {
        return Second::Get();
    };
};
