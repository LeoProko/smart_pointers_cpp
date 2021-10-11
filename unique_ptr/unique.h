#pragma once

// Leo Proko
// leoprokowork@gmail.com

#include "compressed_pair.h"

#include <algorithm>
#include <cstddef>
#include <memory>

template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtr {
private:
    CompressedPair<T*, Deleter> ptr_pair_;

    template <typename U, typename D>
    friend class UniquePtr;

public:
    explicit UniquePtr(T* ptr = nullptr) : ptr_pair_(ptr, Deleter{}){
    }
    
    template <typename D>
    UniquePtr(T* ptr, D&& deleter) : ptr_pair_(ptr, std::forward<D>(deleter)) {
    }

    UniquePtr(const UniquePtr& other) = delete;

    UniquePtr(UniquePtr&& other) noexcept  {
        ptr_pair_.GetFirst() = other.ptr_pair_.GetFirst();
        other.ptr_pair_.GetFirst() = nullptr;

        ptr_pair_.GetSecond() = std::forward<Deleter>(other.ptr_pair_.GetSecond());
    }

    template <typename F, typename D>
    UniquePtr(UniquePtr<F, D>&& other) noexcept {
        ptr_pair_.GetFirst() = other.ptr_pair_.GetFirst();
        other.ptr_pair_.GetFirst() = nullptr;

        ptr_pair_.GetSecond() = std::forward<D>(other.ptr_pair_.GetSecond());
    }

    UniquePtr& operator=(const UniquePtr& other) = delete;


    UniquePtr& operator=(UniquePtr&& other) noexcept {
        Reset(other.Release());
        ptr_pair_.GetSecond() = std::forward<Deleter>(other.ptr_pair_.GetSecond());
        return *this;
    }

    template <typename F, typename D>
    UniquePtr& operator=(UniquePtr<F, D>&& other) noexcept {
        Reset(other.Release());
        ptr_pair_.GetSecond() = std::forward<D>(other.ptr_pair_.GetSecond());
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        Reset();
        return *this;
    }

    ~UniquePtr() {
        Reset();
    }

    T* Release() {
        auto temp = Get();
        ptr_pair_.GetFirst() = nullptr;
        return temp;
    }

    void Reset(T* ptr = nullptr) {
        if (ptr != Get()) {
            auto temp = Get();
            ptr_pair_.GetFirst() = ptr;
            ptr_pair_.GetSecond()(temp);
        }
    }

    void Swap(UniquePtr& other) {
        std::swap(ptr_pair_.GetFirst(), other.ptr_pair_.GetFirst());
    }

    T* Get() const {
        return ptr_pair_.GetFirst();
    }

    Deleter& GetDeleter() {
        return ptr_pair_.GetSecond();
    }

    const Deleter& GetDeleter() const {
        return ptr_pair_.GetSecond();
    }

    explicit operator bool() const {
        return Get();
    }

    std::add_lvalue_reference_t<T> operator*() const {
        return *Get();
    }

    T* operator->() const {
        return Get();
    }
};

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
private:
    CompressedPair<T*, Deleter> ptr_pair_;

    template <typename U, typename D>
    friend class UniquePtr;

public:
    explicit UniquePtr(T* ptr = nullptr) : ptr_pair_(ptr, Deleter{}){
    }
    
    template <typename D>
    UniquePtr(T* ptr, D&& deleter) : ptr_pair_(ptr, std::forward<D>(deleter)) {
    }

    UniquePtr(const UniquePtr& other) = delete;

    UniquePtr(UniquePtr&& other) noexcept  {
        ptr_pair_.GetFirst() = other.ptr_pair_.GetFirst();
        other.ptr_pair_.GetFirst() = nullptr;

        ptr_pair_.GetSecond() = std::forward<Deleter>(other.ptr_pair_.GetSecond());
    }

    template <typename F, typename D>
    UniquePtr(UniquePtr<F, D>&& other) noexcept {
        ptr_pair_.GetFirst() = other.ptr_pair_.GetFirst();
        other.ptr_pair_.GetFirst() = nullptr;

        ptr_pair_.GetSecond() = std::forward<D>(other.ptr_pair_.GetSecond());
    }

    UniquePtr& operator=(const UniquePtr& other) = delete;


    UniquePtr& operator=(UniquePtr&& other) noexcept {
        Reset(other.Release());
        ptr_pair_.GetSecond() = std::forward<Deleter>(other.ptr_pair_.GetSecond());
        return *this;
    }

    template <typename F, typename D>
    UniquePtr& operator=(UniquePtr<F, D>&& other) noexcept {
        Reset(other.Release());
        ptr_pair_.GetSecond() = std::forward<D>(other.ptr_pair_.GetSecond());
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        Reset();
        return *this;
    }

    ~UniquePtr() {
        Reset();
    }

    T* Release() {
        auto temp = Get();
        ptr_pair_.GetFirst() = nullptr;
        return temp;
    }

    void Reset(T* ptr = nullptr) {
        if (ptr != Get()) {
            auto temp = Get();
            ptr_pair_.GetFirst() = ptr;
            ptr_pair_.GetSecond()(temp);
        }
    }

    void Swap(UniquePtr& other) {
        std::swap(ptr_pair_.GetFirst(), other.ptr_pair_.GetFirst());
    }

    T* Get() const {
        return ptr_pair_.GetFirst();
    }

    Deleter& GetDeleter() {
        return ptr_pair_.GetSecond();
    }

    const Deleter& GetDeleter() const {
        return ptr_pair_.GetSecond();
    }

    explicit operator bool() const {
        return Get();
    }

    std::add_lvalue_reference_t<T> operator[](int index) {
        return Get()[index];
    }
    const std::add_lvalue_reference_t<T> operator[](int index) const {
        return Get()[index];
    }
};
