#pragma once

#include "core/status.h"
#include "core/stdlib/type_traits.h"

namespace rtk {
template <typename T>
class ServiceProvider;
class ServiceContext;

template <typename T>
class ServiceProvider {
 public:
  ServiceProvider(ServiceContext& context) : context_{context} {}
  virtual ~ServiceProvider() noexcept = 0;
  virtual T& getInstance() = 0;
  virtual const T& getInstance() const = 0;

 protected:
  ServiceContext& context_;
};  // class ServiceProvider

// Specialization: SFINAE succeeds if ServiceProvider<T> is complete
template <typename T>
struct is_service_available<T, void_t<decltype(sizeof(ServiceProvider<T>))>>
    : true_type {};
template <typename, typename = void>
struct is_service_available : false_type {};

class ServiceContext {
 public:
  template <typename T>
  const T* get() const {
    if constexpr (is_service_available<T>::value) {
      ServiceProvider<T> t{*this};
      return &t.getInstance();
    } else {
      return nullptr;
    }
  }
  template <typename T>
  bool has() const {
    if constexpr (is_service_available<T>::value) {
      ServiceProvider<T> t{*this};
      return get() != nullptr;
    } else {
      return false;
    }
  }
  virtual ~ServiceContext() noexcept = 0;
  ServiceContext(const ServiceContext& other) = delete;
  ServiceContext(ServiceContext&& other) = delete;
  ServiceContext& operator=(const ServiceContext& other) = delete;
  ServiceContext& operator=(const ServiceContext&& other) = delete;

 protected:
  ServiceContext() = default;
};  // class ServiceContainer

StatusCode CreateServices();

template <typename T>
inline ServiceProvider<T>::~ServiceProvider() noexcept {}

inline ServiceContext::~ServiceContext() noexcept {}

}  // namespace rtk