#pragma once

#include "core/stdlib/memory.h"
#include "core/stdlib/type_traits.h"  // rtk::is_same
#include "core/stdlib/utility.h"      // rtk::swap
#include "test/test.h"

static constexpr auto kInitialValue = 0xabadbabe;

class initialized {
 public:
  int initialValue = kInitialValue;
};

class deleteable {
 public:
  bool& deleted;
  deleteable(bool& deleted) : deleted{deleted = false} {}
  ~deleteable() { deleted = true; }
};

class fake_deleteable {
 public:
  bool deleted = false;
  bool destructed = false;
  ~fake_deleteable() { destructed = true; }
};

template <class T>
class fake_deleter {};
template <>
class fake_deleter<fake_deleteable> {
 public:
  // Fake deleter doesn't call destructor or delete, only sets "deleted" variable
  void operator()(fake_deleteable* obj) const {
    if (obj == nullptr)
      return;  // Don't delete nonexistent objects!
    obj->deleted = true;
  }
};

int test_unique_default_constructor() {
  // Make sure default-constructed object is initialized properly
  auto init = rtk::unique_ptr<initialized>{};
  EXPECT_EQUAL(init.get()->initialValue, kInitialValue);
  return 0;
}

int test_unique_default() {
  // For fun create uninitialized
  alignas(initialized) char raw[sizeof(initialized)];
  auto notinit = reinterpret_cast<initialized*>(&raw);
  EXPECT_NOT_EQUAL(notinit->initialValue, kInitialValue);

  // Make sure default-constructed object is initialized properly
  auto init = rtk::make_unique<initialized>();
  EXPECT_EQUAL(init.get()->initialValue, kInitialValue);
  return 0;
}

int test_unique_deleted() {
  bool deleted = false;
  { rtk::make_unique<deleteable>(deleted); }
  EXPECT_TRUE(deleted);
  return 0;
}

int test_unique_from_pointer() {
  bool deleted;
  { rtk::unique_ptr(new deleteable{deleted}); }
  EXPECT_TRUE(deleted);  // good, assumed deleted with `delete`
  return 0;
}

int test_unique_get() {
  bool deleted;
  {
    auto ptr = rtk::make_unique<deleteable>(deleted);
    EXPECT_FALSE(ptr.get()->deleted);
  }
  EXPECT_TRUE(deleted);
  return 0;
}

int test_unique_custom_deleter() {
  // use stack, not new
  fake_deleteable d;
  {
    auto ptr =
        rtk::unique_ptr<fake_deleteable, fake_deleter<fake_deleteable>>(&d);
    EXPECT_FALSE(d.deleted);
    EXPECT_FALSE(d.destructed);
    // it's ok, we're not actually "deleting" our stack-allocated thing
  }
  EXPECT_FALSE(d.destructed);  // fake deleter doesn't call deconstructor;
  EXPECT_TRUE(d.deleted);
  return 0;
}

int test_unique_release() {
  bool deleted;
  auto unique = rtk::make_unique<deleteable>(deleted);
  EXPECT_NONNULL(unique.get());
  EXPECT_FALSE(deleted);
  auto released = unique.release();
  EXPECT_NULL(unique.get());
  EXPECT_FALSE(deleted);

  delete released;
  EXPECT_TRUE(deleted);
  return 0;
}

int test_unique_reset() {
  auto a = fake_deleteable();
  auto b = fake_deleteable();
  auto unique =
      rtk::unique_ptr<fake_deleteable, fake_deleter<fake_deleteable>>{&a};
  EXPECT_EQUAL(unique.get(), &a);
  EXPECT_FALSE(a.deleted);

  unique.reset(&b);
  EXPECT_EQUAL(unique.get(), &b);
  EXPECT_TRUE(a.deleted);
  EXPECT_FALSE(a.destructed);  // fake deleter does not call
  EXPECT_FALSE(b.deleted);

  unique.reset();
  EXPECT_NULL(unique.get());
  EXPECT_TRUE(b.deleted);
  EXPECT_FALSE(b.destructed);  // fake deleter does not call
  return 0;
}

int test_unique_move_construct() {
  auto ptr1 = rtk::make_unique<int>(kInitialValue);
  auto ptr2{rtk::move(ptr1)};
  EXPECT_NULL(ptr1.get());
  EXPECT_NONNULL(ptr2.get());
  EXPECT_EQUAL(*ptr2.get(), kInitialValue);
  return 0;
}

int test_unique_move_assign() {
  auto ptr1 = rtk::make_unique<int>(kInitialValue);
  auto ptr2 = rtk::move(ptr1);
  EXPECT_NULL(ptr1.get());
  EXPECT_NONNULL(ptr2.get());
  EXPECT_EQUAL(*ptr2.get(), kInitialValue);

  // Now check for overwriting move assign
  auto ptr3 = rtk::make_unique<int>(kInitialValue);
  ptr3 = rtk::move(ptr2);
  EXPECT_NULL(ptr2.get());
  EXPECT_NONNULL(ptr3.get());
  EXPECT_EQUAL(*ptr3.get(), kInitialValue);

  return 0;
}

int test_unique_move_assign_same() {
  auto ptr = rtk::make_unique<int>(kInitialValue);
  ptr = move(ptr);
  EXPECT_NONNULL(ptr.get());
  EXPECT_EQUAL(*ptr.get(), kInitialValue);

  return 0;
}
int test_unique_swap() {
  //using rtk::swap; // force `friend` version
  auto a = rtk::make_unique<int>(4);
  auto b = rtk::make_unique<int>(5);
  // static_assert(!rtk::is_same<decltype(&rtk::swap<rtk::unique_ptr<int>>),
  //                             decltype(&swap<rtk::unique_ptr<int>>)>::value);
  swap(a, b);
  EXPECT_EQUAL(*a.get(), 5);
  EXPECT_EQUAL(*b.get(), 4);

  // Try non-friend version. Should it work?
  rtk::swap(a, b);
  EXPECT_EQUAL(*a.get(), 4);
  EXPECT_EQUAL(*b.get(), 5);
  return 0;
}

int test_unique_dereference() {
  auto ptr = rtk::make_unique<int>(kInitialValue);
  EXPECT_EQUAL(*ptr, kInitialValue);
  EXPECT_EQUAL(*ptr.operator->(), kInitialValue);

  // const versions
  const auto& cptr = ptr;
  EXPECT_EQUAL(*cptr, kInitialValue);
  EXPECT_EQUAL(*cptr.operator->(), kInitialValue);

  return 0;
}

int test_unique_bool() {
  auto ptr = rtk::make_unique<int>(kInitialValue);
  bool result = ptr;
  EXPECT_TRUE(result);
  result = !ptr;
  EXPECT_FALSE(result)

  ptr.reset();
  result = ptr;
  EXPECT_FALSE(result);
  result = !ptr;
  EXPECT_TRUE(result);

  return 0;
}

int test_unique_null() {
  int* ptr = nullptr;
  auto a = rtk::make_unique<int>(ptr);
  EXPECT_NULL(a.get());

  auto b = rtk::make_unique<int>(nullptr);
  EXPECT_NULL(b.get());

  auto c = rtk::unique_ptr<int>{nullptr};
  EXPECT_NULL(c.get());
  return 0;
}

void stdlib_memory_tests() {
  TEST(test_unique_default_constructor);
  TEST(test_unique_default);
  TEST(test_unique_deleted);
  TEST(test_unique_from_pointer);
  TEST(test_unique_get);
  TEST(test_unique_custom_deleter);
  TEST(test_unique_release);
  TEST(test_unique_reset);
  TEST(test_unique_move_construct);
  TEST(test_unique_move_assign);
  TEST(test_unique_move_assign_same);
  TEST(test_unique_swap);
  TEST(test_unique_dereference);
  TEST(test_unique_bool);
  TEST(test_unique_null);
}