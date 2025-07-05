#pragma once

#include <stdatomic.h>
#include <initializer_list>
#include "core/stdlib/stdlib.h"   // rtk::panic
#include "core/stdlib/utility.h"  // rtk::swap, rtk::as_const

namespace rtk {
template <class T>
class heap_deleter final {
 public:
  // `delete` is assumed noop for null
  void operator()(T* obj) const { delete obj; }
};  // class heap_deleter

template <class T>
class heap_deleter<T[]> final {
 public:
  // `delete` is assumed noop for null
  void operator()(T* obj) const { delete[] obj; }
};  // class heap_deleter

// // Don't use for arrays
// // Array specializations of templates (T[])
// // will use the array version
// template <typename T, typename... Args>
// class heap_allocator final {
//  public:
//   // Null check responsibility on caller
//   // Don't use for arrays
//   // `new` should gracefully return null upon failure
//   T* alloc(Args&&...) { return new T{Args...}; }
// };  // class heap_allocator<T, ...Args>

// template <typename T>
// class heap_array_allocator final {
//  public:
//   // Null check responsibility on caller
//   // `new` should gracefully return null upon failure,
//   // and destruct any objects already allocated
//   // Destructed objects should not leave dangling
//   // pointers when moved.
//   T* alloc(size_t count) {
//     return new T[count]{};  // value initialized
//   }
// };  // class heap_array_allocator<T>

template <class T, class Deleter>
class unique_ptr_base {
 public:
  using BaseType = T;
  // Can't copy-- it's what makes it unique
  unique_ptr_base(const unique_ptr_base& other) = delete;
  unique_ptr_base& operator=(const unique_ptr_base& other) = delete;
  // Must also be defined in derived classes for consistency
  friend void swap(unique_ptr_base& a, unique_ptr_base& b) noexcept {
    using rtk::swap;

    swap(a.ptr_, b.ptr_);
  }
  // Probably should be redefined in derived class as well
  unique_ptr_base(unique_ptr_base&& other) noexcept {
    // swap(*this, other); // don't!!!
    ptr_ = other.ptr_;
    other.ptr_ = nullptr;  // dangling pointer
  }
  // Probably should be redefined in derived class as well
  unique_ptr_base& operator=(unique_ptr_base&& other) noexcept {
    if (this != &other) {
      // swap(*this, other); // don't!!!
      ptr_ = other.ptr_;
      other.ptr_ = nullptr;
    }
    return *this;
  }

  void reset(T* new_ptr = nullptr) {
    // Null delete should be safe; no-op
    Deleter{}(ptr_);
    ptr_ = new_ptr;
  }

  // Caller is responsible for null checks, taking ownership,
  // etc.
  T* release() {
    auto oldptr = ptr_;
    ptr_ = nullptr;  // leave in valid empty state
                     // reflecting relinquished ownership
    return oldptr;
  }

  // Caller does not own pointer and must resolve dangling pointers
  const T* get() const { return ptr_; }
  // Caller does not own pointer and must resolve dangling pointers
  T* get() { return ptr_; }
  // Caller must avoid dangling references by keeping them in scope as they cannot be cleared
  const T& operator*() const { return *ptr_; }
  // Caller responsible for null check
  T& operator*() { return *ptr_; }
  // Caller responsible for null check
  const T* operator->() const { return ptr_; }
  // Caller responsible for null check
  T* operator->() { return ptr_; }

  bool operator!() const { return ptr_ == nullptr; }
  operator bool() const { return ptr_ != nullptr; }

  ~unique_ptr_base() {
    // Assumed safe, no-op on null
    Deleter{}(ptr_);
  }

 protected:
  T* ptr_;  // Owned and managed by this unique_ptr
  unique_ptr_base() : ptr_{new T{}} {}
  // Caller must avoid dangling pointers after relinquishing ownership to unique_ptr
  // null pointers are valid for safety
  unique_ptr_base(T* ptr) : ptr_{ptr} {}
  // don't do this-- what if you accidentally pass an lvalue instead of pointer?
  // then the object will be copy constructed using this overload instead of (T* ptr)
  // template <typename... Args>
  // unique_ptr_base(Args&&... args) : ptr_{new T{rtk::forward<Args>(args)...}} {}
};  // class unique_ptr_base<T, deleter>

template <class T, class Deleter = heap_deleter<T>>
class unique_ptr : public unique_ptr_base<T, Deleter> {
 public:
  using Base = unique_ptr_base<T, Deleter>;
  unique_ptr() : Base() {}
  // Caller must avoid dangling pointers after relinquishing ownership to unique_ptr
  // null pointers are valid for safety
  unique_ptr(T* ptr) : Base{ptr} {}
  // Defined in derived class for consistency
  friend void swap(unique_ptr& a, unique_ptr& b) noexcept {
    using rtk::swap;

    swap(a.ptr_, b.ptr_);
  }

  // don't do this-- what if you accidentally pass an lvalue instead of pointer?
  // then the object will be copy constructed using this overload instead of (T* ptr)
  // template <typename... Args>
  // unique_ptr(Args&&... args) : Base(rtk::forward<Args>(args)...) {}
};  // class unique_ptr

// Deleter evaluates to a T[] specialization, which should call delete[]
// or similar for proper array destruction.
template <class T, class Deleter>
class unique_ptr<T[], Deleter> : public unique_ptr_base<T, Deleter> {
 public:
  using Base = unique_ptr_base<T, Deleter>;
  unique_ptr() : Base(), len_{0} {}

  // Caller must avoid dangling pointers after relinquishing ownership to unique_ptr
  // null pointers are valid for safety
  // length is properly set to 0 if the pointer is null.
  unique_ptr(T* ptr, size_t len) : Base(ptr), len_{ptr ? len : 0} {}
  // use make_unique_array instead
  // unique_ptr(std::initializer_list<T> args) : len_{0} {
  //   // Must fail gracefully when out of memory; return on nullptr
  //   // unique_ptr is taking ownership of new array
  //   auto newarr = new T[args.size()];
  //   if (newarr == nullptr)
  //     return;
  //   size_t i = 0;
  //   for (const T& val : args) {
  //     newarr[i++] = val;
  //   }
  //   ptr_ = newarr;
  //   len_ = args.size();
  // }

  explicit unique_ptr(size_t len) {
    // Don't try to allocate an empty array
    if (len == 0) {
      len_ = 0;
      ptr_ = nullptr;
      return;
    }
    auto newarr = new T[len];
    ptr_ =
        newarr;  // can be null on out-of-memory; otherwise takes ownership of new object
    len_ = newarr ? len : 0;  // proper null check on out-of-memory
  }
  // Redefined for consistency with added length
  friend void swap(unique_ptr<T[], Deleter>& a,
                   unique_ptr<T[], Deleter>& b) noexcept {
    using rtk::swap;

    swap(static_cast<Base&>(a), static_cast<Base&>(b));
    swap(a.len_, b.len_);
  }
  // Overridden because we must swap length as well
  unique_ptr(unique_ptr&& other) noexcept { swap(*this, other); }
  // Overridden because we must swap length as well
  unique_ptr& operator=(unique_ptr&& other) noexcept {
    if (this != &other) {
      swap(*this, other);
    }
    return *this;
  }
  size_t length() const { return ptr_ ? len_ : 0; }
  // Caller responsible for bounds and null checking. Will trap on error.
  // Should encourage status checking function instead
  const T& operator[](size_t index) const {
    // len_ is inclusive
    auto ptr = index < len_ ? ptr_ : nullptr;
    auto i = index < len_ ? index
                          : 0;  // forces nullptr[0] instead of nullptr[index]
    // Try to fail gracefully with null dereference instead
    // if (index >= len_) panic("Out of bounds!");
    // use safe i
    return ptr[i];
  }
  // Non-const access delegates to const logic for safety and DRYness.
  // Mutation is explicitly granted via const_cast.
  T& operator[](size_t index) {
    return const_cast<T&>(rtk::as_const(*this)[index]);
  }
  void reset(T*) = delete;  // TODO: change from hack
  void reset() { static_cast<Base*>(this)->reset(); }
  void reset(T* ptr, size_t len) {
    reset();
    len_ = len;
  }

 private:
  using Base::ptr_;
  size_t len_;
};  // class unique_ptr<T[], Deleter>

// make_unique()
// Heap should cause unique_ptr to evaluate to null gracefully
// when out of memory
template <typename T, typename... Args>
unique_ptr<T, heap_deleter<T>> make_unique(Args&&... args) {
  return unique_ptr<T, heap_deleter<T>>{new T(rtk::forward<Args>(args)...)};
}
template <typename T>
unique_ptr<T, heap_deleter<T>> make_unique(std::nullptr_t) {
  return unique_ptr<T, heap_deleter<T>>{nullptr};
}

template <typename T>
unique_ptr<T[]> make_unique_array(std::initializer_list<T> args) {
  auto ptr = new T[args.size()]{};
  auto i = 0;
  for (auto& arg : args) {
    // copy (force lvalue reference)
    ptr[i++] = arg;
  };
  return unique_ptr<T[]>{ptr, args.size()};
}

// Caller is responsible for clearing dangling pointers and
// relinquishing ownership. Do not use for array types.
// Heap should cause unique_ptr to evaluate to null gracefully
// when out of memory
template <typename T>
unique_ptr<T, heap_deleter<T>> make_unique(T* obj) {
  return unique_ptr<T, heap_deleter<T>>{obj};
}

template <typename T>
unique_ptr<T[], heap_deleter<T[]>> make_unique(T* obj, size_t len) {
  return unique_ptr<T[], heap_deleter<T[]>>{obj, len};
}

// TODO: make_unique for arrays

// template <typename T, typename Deleter = heap_deleter<T>>
// class shared_ptr {
//  private:
//   class control_block<T, Deleter>;
//   using CB = control_block<T, Deleter>;

//  public:
//   shared_ptr() : reference_{nullptr} {};
//   shared_ptr(T* ptr) { reference_ = new Reference{ptr}; }
//   virtual ~shared_ptr() noexcept {
//     if (reference_ != nullptr) {
//       reference_->removeRef();
//     }
//   }
//   shared_ptr(const shared_ptr& other) { *this = other; }
//   shared_ptr(shared_ptr&& other) { swap(*this, other); }
//   shared_ptr& operator=(const shared_ptr& other) {
//     if (this != &other) {
//       other.reference_->addRef();
//       if (reference_ != nullptr) {
//         ifreference_->removeRef();
//       }
//       reference_ = other.reference_;
//     }
//   }
//   shared_ptr&& operator=(shared_ptr&& other) noexcept { swap(*this, other); }
//   friend void swap(shared_ptr& a, shared_ptr& b) noexcept {
//     using k::swap;

//     swap(a.reference_, b.reference_);
//   }

//   const T* get() const { return reference_->ptr_; }
//   T* get() { return ptr_; }
//   const T& operator*() const { return *ptr_; }
//   T& operator*() { return *ptr_; }
//   const T* operator->() const { return ptr_; }
//   T& operator->() { return ptr_; }

//   bool operator!() const { return ptr_ == nullptr; }
//   bool operator bool() const { return ptr_ != nullptr; }

//  protected:
//  private:
//   class Reference final {
//    public:
//     Reference(T* p) : ptr_{p}, reference_count_{1} {}

//     void addRef() { atomic_fetch_add(&reference_count_, 1); }
//     bool removeRef() {
//       if (reference_count_ <= 0 || ptr_ == nullptr)
//         return;
//       auto prev = atomic_fetch_sub(&reference_count_, 1);
//       if (prev == 1) {
//         Deleter t;
//         t(ptr_);  // Delete the object
//         ptr_ = nullptr;
//         delete this;  // Delete the reference counter
//         return true;
//       }
//       return false;
//     }

//    private:
//     T* ptr_;
//     atomic_int reference_count_;
//   }* reference_;
// };

}  // namespace rtk