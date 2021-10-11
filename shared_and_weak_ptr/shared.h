#pragma once

// Leo Proko
// leoprokowork@gmail.com

#include "sw_fwd.h"

#include <cstddef>
#include <type_traits>

class BlockBase {
public:
    int shared_ref_counter_{};
    int weak_ref_counter_{};

    BlockBase() : shared_ref_counter_(1), weak_ref_counter_(0) {
    }

    virtual void Clear() = 0;

    virtual ~BlockBase() = default;
};

template <typename T>
class Block : public BlockBase {
public:
    T* ptr_;

    Block(T* ptr) : ptr_(ptr) {
    }
    
    void Clear() override {
        delete ptr_;
        ptr_ = nullptr;
    }

    ~Block() override {
        Clear();
    }
};

template <typename T>
class BlockHolder : public BlockBase {
public:
    std::aligned_storage_t<sizeof(T), alignof(T)> storage_;

    template <typename... Args>
    BlockHolder(Args&&... args) {
        new (&storage_) T(std::forward<Args>(args)...);
    }

    T* GetRawPtr() {
        return reinterpret_cast<T*>(&storage_);
    }

    void Clear() override {
        reinterpret_cast<T*>(&storage_)->~T();
    }

    ~BlockHolder() override {
        Clear();
    }
};

template <typename T>
class SharedPtr {
private:
    T* ptr_;
    BlockBase* block_;

    void Clear() noexcept {
        if (block_) {
            if (block_->shared_ref_counter_ > 1) {
                --block_->shared_ref_counter_;
            } else {
                if (block_->weak_ref_counter_ < 1) {
                    delete block_;
                } else {
                    block_->Clear();
                    --block_->shared_ref_counter_;
                }
            }
        }
        ptr_ = nullptr;
        block_ = nullptr;
    }

    void IncreaseRefCounter() {
        if (block_) {
            ++block_->shared_ref_counter_;
        }
    }

    template <typename U, typename... Args>
    friend SharedPtr<U> MakeShared(Args&&... args);

    template <typename U>
    friend class SharedPtr;

    template <typename U>
    friend class WeakPtr;

public:
    SharedPtr() : ptr_(nullptr), block_(nullptr) {
    }
    SharedPtr(std::nullptr_t) : ptr_(nullptr), block_(nullptr) {
    }

    template <typename U>
    explicit SharedPtr(U* ptr) : ptr_(ptr), block_(new Block<U>(ptr)) {
    }

    SharedPtr(const SharedPtr& other) : ptr_(other.ptr_), block_(other.block_) {
        IncreaseRefCounter();
    }

    template <typename U>
    SharedPtr(const SharedPtr<U>& other) : ptr_(other.ptr_), block_(other.block_) {
        IncreaseRefCounter();
    }

    SharedPtr(SharedPtr&& other) : ptr_(std::move(other.ptr_)), block_(std::move(other.block_)) {
        IncreaseRefCounter();
        other.Clear();
    }

    template <typename U>
    SharedPtr(SharedPtr<U>&& other) : ptr_(std::move(other.ptr_)), block_(std::move(other.block_)) {
        IncreaseRefCounter();
        other.Clear();
    }

    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) : ptr_(ptr), block_(other.block_) {
        IncreaseRefCounter();
    }

    explicit SharedPtr(const WeakPtr<T>& other) : ptr_(other.ptr_), block_(other.block_) {
        if (!block_) {
            throw BadWeakPtr();
        }
        IncreaseRefCounter();
    }

    ~SharedPtr() noexcept {
        Clear();
    }

    SharedPtr& operator=(const SharedPtr& other) {
        Clear();
        ptr_ = other.ptr_;
        block_ = other.block_;
        IncreaseRefCounter();
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) {
        Clear();
        ptr_ = std::move(other.ptr_);
        block_ = std::move(other.block_);
        IncreaseRefCounter();
        return *this;
    }

    void Reset(T* ptr = nullptr) {
        if (ptr != Get()) {
            Clear();
            if (ptr) {
                ptr_ = ptr;
                block_ = new Block<T>(ptr);
            }
        }
    }

    template <typename U>
    void Reset(U* ptr = nullptr) {
        if (ptr != Get()) {
            Clear();
            if (ptr) {
                ptr_ = ptr;
                block_ = new Block<U>(ptr);
            }
        }
    }

    void Swap(SharedPtr& other) {
        std::swap(ptr_, other.ptr_);
        std::swap(block_, other.block_);
    }

    T* Get() const {
        return ptr_;
    }

    T* operator->() const {
        return Get();
    }

    T& operator*() const {
        return *ptr_;
    }

    size_t UseCount() const {
        if (block_) {
            return block_->shared_ref_counter_;
        }
        return 0;
    }

    explicit operator bool() const {
        return ptr_;
    }
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right) {
    return left.ptr_ == right.ptr_ && left.block_ == right.block_;
}

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    SharedPtr<T> sp;
    auto block = new BlockHolder<T>(std::forward<Args>(args)...);
    sp.block_ = block;
    sp.ptr_ = block->GetRawPtr();
    return sp;
}
