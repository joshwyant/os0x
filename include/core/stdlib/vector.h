#pragma once

#include <cstddef>
#include <cstdint>
#include "core/status.h"
#include "core/stdlib/memory.h"

namespace rtk {
template <typename T>
class vector {
  mutable size_t size_ = 0;
  mutable size_t initial_capacity_ = default_initial_capacity_;
  mutable size_t capacity_ = 0;
  mutable unique_ptr<T[]> elems_;  // resize const data

 public:
  vector() : size_{0}, capacity_{0}, elems_{} {}
  explicit vector(size_t size) : size_{size} {}
  explicit vector(size_t size, size_t initial_capacity)
      : size_{size}, initial_capacity_{initial_capacity} {}

  const size_t size() const { return size_; }
  const T& operator[](size_t index) const {
    if (index >= size_) {
      TRAP(OutOfBounds);
    }
    if (capacity_ == 0) {
      resize(size_);  // accessing empty vector before buffer is created
    }
    return elems_[index];
  }
  T& operator[](size_t index) {
    return const_cast<T&>(rtk::as_const(*this)[index]);
  }
  template <typename... Args>
  void emplace_back(Args&&... args) {
    auto old_size = size_;
    resize(size_ + 1);
    if (size_ <= old_size) {
      TRAP(OutOfMemory);
      return;
    }
    T* ptr = elems_.get() + old_size;
    new (ptr) T{move(args)...};
  }
  // Push by moving
  void push_back(T&& elem) {
    auto old_size = size_;
    resize(size_ + 1);
    if (size_ <= old_size) {
      TRAP(OutOfMemory);
      return;
    }
    elems_[old_size] = move(elem);
  }
  // Push by copying
  void push_back(const T& elem) {
    auto old_size = size_;
    resize(size_ + 1);
    if (size_ <= old_size) {
      TRAP(OutOfMemory);
      return;
    }
    elems_[old_size] = elem;
  }
  void pop_back() {
    if (size_ == 0)
      return;
    resize(size_ - 1);
  }
  void resize(size_t new_size) const {  // not logically mutating the list
    if (new_size > size_) {
      // reserve capacity
      grow(new_size);
    } else if (new_size < size_) {
      // Retire old objects
      for (auto i = new_size; i < size_; i++) {
        elems_[i].~T();
      }
      size_ = new_size;
    }
  }
  void reserve(size_t capacity) {  // not logically mutating the list
    if (capacity == 0) {
      return;
    }
    if (capacity_ == 0) {
      // Lazy reserve--only setting initial capacity!
      initial_capacity_ = capacity;
    } else {
      // grow only
      grow(capacity);
    }
  }

 private:
  friend class vector_tests;

  static constexpr size_t num_ = 3;
  static constexpr size_t denom_ = 2;
  static constexpr size_t default_initial_capacity_ = 4;

  static_assert(denom_ != 0, "growth denominator is 0");
  static_assert(num_ > denom_, "growth function is inverted");
  static_assert(2 * num_ / denom_ > 2, "size must increase nominally");
  static_assert(num_ / denom_ <= 2, "ratio is too huge");
  static_assert(default_initial_capacity_ > 0,
                "Must have capacity of at least 1");

  template <size_t num, size_t denom>
  size_t computeGrowth() const {
    auto new_capacity =
        capacity_ == 0 ? initial_capacity_ : num * capacity_ / denom;
    if (new_capacity == capacity_) {
      new_capacity += 1;
    }
    return new_capacity;
  }
  size_t computeGrowth() const { return computeGrowth<num_, denom_>(); }
  // Returns whether a resize was needed
  bool grow(size_t requested_size) const {  // doesn't logically affect data
    // Start using existing capacity
    auto new_capacity = capacity_;
    // See if we can bump up to the next ideal capacity
    if (requested_size > capacity_) {
      new_capacity = computeGrowth();
    }
    // See if we need more than a small bump
    if (requested_size > new_capacity) {
      new_capacity = requested_size;
    }
    // See if we grew at all
    if (new_capacity <= capacity_) {
      size_ = requested_size;
      return false;
    }
    // Make a bigger array
    unique_ptr<T[]> new_elems(new_capacity);
    // Check success
    if (!new_elems) {
      TRAP(OutOfMemory);
      return false;  // leave unmodified
    }
    // Capacity has been increased
    capacity_ = new_capacity;
    // Use fast indexing
    auto prevptr = elems_.get();
    auto ptr = new_elems.get();
    // Copy or move items
    for (auto i = 0; i < size_; i++) {
      ptr[i] = move(prevptr[i]);
    }
    // Need to initialize remaining?
    for (auto i = size_; i < new_capacity; i++) {
      new (ptr + i) T{};
    }
    // Move, and old pointer deallocates array
    elems_ = move(new_elems);
    // New size is now correct
    size_ = requested_size;
    // We resized
    return true;
  }
};  // class vector
}  // namespace rtk