#pragma once

#include <stdatomic.h>
#include "kernel.h"
#include "kernel/lib/cpp/utility.h"  // k::swap

namespace k {
template <class T>
class heap_deleter final {
 public:
  void operator()(T* obj) const { delete obj; }
};  // class heap_deleter

template <typename T, typename... Args>
class heap_allocator final {
 public:
  T* operator()(Args&&...) { return new T{Args...}; }
};  // class heap_allocator

template <class T, class TDeleter = heap_deleter<T>>
class unique_ptr {
 public:
  unique_ptr() : ptr_{nullptr} {}
  unique_ptr(T* ptr) : ptr_{ptr} {}
  virtual ~unique_ptr() noexcept {
    // TODO: Use a mutex
    TDeleter d;
    d(ptr_);
  }
  // Can't copy-- it's what makes it unique
  unique_ptr(const unique_ptr<T>& other) = delete;
  unique_ptr<T>& operator=(const unique_ptr<T>& other) = delete;
  friend void swap(unique_ptr<T>& a, unique_ptr<T>& b) noexcept {
    using k::swap;

    swap(a.ptr_, b.ptr_);
  }
  unique_ptr(unique_ptr<T>&& other) noexcept { swap(*this, other); }
  unique_ptr<T>& operator=(unique_ptr<T>&& other) noexcept {
    if (this != &other) {
      swap(*this, other);
    }
    return *this;
  }

  void reset(T* new_ptr = nullptr) {
    TDeleter d;
    d(ptr_);
    ptr_ = new_ptr;
  }

  T* release() {
    auto oldptr = ptr_;
    ptr_ = nullptr;
    return oldptr;
  }

  const T* get() const { return ptr_; }
  T* get() { return ptr_; }
  const T& operator*() const { return *ptr_; }
  T& operator*() { return *ptr_; }
  const T* operator->() const { return ptr_; }
  T& operator->() { return ptr_; }

  bool operator!() const { return ptr_ == nullptr; }
  bool operator bool() const { return ptr_ != nullptr; }

 protected:
  T* ptr_;
};  // class unique_ptr<T, deleter>

template <typename T, typename... Args>
unique_ptr<T>&& make_unique() {
  return unique_ptr<T>{new T{Args && ... }};
}
template <typename T, typename... Args>
unique_ptr<T>&& make_unique(T* obj) {
  return unique_ptr<T>{obj};
}

template <typename T, typename TDeleter = heap_deleter<T>>
class shared_ptr {
 public:
  shared_ptr() : reference_{nullptr} {};
  shared_ptr(T* ptr) { reference_ = new Reference{ptr}; }
  virtual ~shared_ptr() noexcept {
    if (reference_ != nullptr) {
      reference_->removeRef();
    }
  }
  shared_ptr(const shared_ptr& other) { *this = other; }
  shared_ptr(shared_ptr&& other) { swap(*this, other); }
  shared_ptr& operator=(const shared_ptr& other) {
    if (this != &other) {
      other.reference_->addRef();
      if (reference_ != nullptr) {
        ifreference_->removeRef();
      }
      reference_ = other.reference_;
    }
  }
  shared_ptr&& operator=(shared_ptr&& other) noexcept { swap(*this, other); }
  friend void swap(shared_ptr& a, shared_ptr& b) noexcept {
    using k::swap;

    swap(a.reference_, b.reference_);
  }

  const T* get() const { return reference_->ptr_; }
  T* get() { return ptr_; }
  const T& operator*() const { return *ptr_; }
  T& operator*() { return *ptr_; }
  const T* operator->() const { return ptr_; }
  T& operator->() { return ptr_; }

  bool operator!() const { return ptr_ == nullptr; }
  bool operator bool() const { return ptr_ != nullptr; }

 protected:
 private:
  class Reference final {
   public:
    Reference(T* p) : ptr_{p}, reference_count_{1} {}

    void addRef() { atomic_fetch_add(&reference_count_, 1); }
    bool removeRef() {
      if (reference_count_ <= 0 || ptr_ == nullptr)
        return;
      auto prev = atomic_fetch_sub(&reference_count_, 1);
      if (prev == 1) {
        TDeleter t;
        t(ptr_);  // Delete the object
        ptr_ = nullptr;
        delete this;  // Delete the reference counter
        return true;
      }
      return false;
    }

   private:
    T* ptr_;
    atomic_int reference_count_;
  }* reference_;
};

}  // namespace k