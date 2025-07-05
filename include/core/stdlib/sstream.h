#pragma once

#include "core/stdlib/ostream.h"
#include "core/stdlib/string.h"

namespace rtk {
class ostringstream : public ostream {
 public:
  ostream& operator<<(string_view data) override {
    str_.append(data);
    return *this;
  }
  ostream& flush() override { return *this; }
  string& str() { return str_; }

 protected:
  string str_;
};  // class ostringstream

template <typename T>
ostringstream& operator<<(ostringstream& stream, T&& data) {
  return static_cast<ostringstream&>(
      static_cast<ostream&>(stream).operator<<(rtk::forward<T>(data)));
}

}  // namespace rtk