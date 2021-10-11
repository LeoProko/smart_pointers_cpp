#pragma once

// Leo Proko
// leoprokowork@gmail.com

#include "sw_fwd.h"
#include "shared.h"

template <typename T>
class WeakPtr {
private:
    T* ptr_;
    BlockBase* block_;

    void Clear() noexcept {
        if (block_) {
            if (block_->weak_ref_counter_ > 1) {
                --block_->weak_ref_counter_;
            } else if (block_->shared_ref_counter_ < 1) {
                delete block_;
            } else {
                --block_->weak_ref_counter_;
            }
        }
        ptr_ = nullptr;
        block_ = nullptr;
    }

    void IncreaseRefCounter() {
        if (block_) {
            ++block_->weak_ref_counter_;
        }
    }

    template <typename U>
    friend class SharedPtr;

public:
    WeakPtr() : ptr_(nullptr), block_(nullptr) {
    }

    WeakPtr(const WeakPtr& other) : ptr_(other.ptr_), block_(other.block_) {
        IncreaseRefCounter();
    }

    WeakPtr(WeakPtr&& other) : ptr_(std::move(other.ptr_)), block_(std::move(other.block_)) {
        IncreaseRefCounter();
        other.Clear();
    }

    WeakPtr(const SharedPtr<T>& other) : ptr_(other.ptr_), block_(other.block_) {
        IncreaseRefCounter();
    }

    WeakPtr& operator=(const WeakPtr& other) {
        Clear();
        ptr_ = other.ptr_;
        block_ = other.block_;
        IncreaseRefCounter();
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) {
        Clear();

        ptr_ = std::move(other.ptr_);
        other.ptr_ = nullptr;

        block_ = std::move(other.block_);
        other.block_ = nullptr;

        IncreaseRefCounter();
        return *this;
    }

    ~WeakPtr() noexcept {
        Clear();
    }

    void Reset() {
        Clear();
    }

    void Swap(WeakPtr& other) {
        std::swap(ptr_, other.ptr_);
        std::swap(block_, other.block_);
    }

    size_t UseCount() const {
        if (block_) {
            return block_->shared_ref_counter_;
        }
        return 0;
    }

    bool Expired() const {
        return UseCount() == 0;
    }

    SharedPtr<T> Lock() const {
        if (Expired()) {
            return SharedPtr<T>();
        }
        return SharedPtr<T>(*this);
    }
};

