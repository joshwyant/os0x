#pragma once

#include "core/stdlib/ostream.h"
#include "core/stdlib/string.h"

namespace rtk {
class ostringstream : public ostream {
 public:
  ostream& write(const char* data) override {
    str_.append(data);
    return *this;
  }
  ostream& flush() override { return *this; }
  string& str() { return str_; }

 protected:
  string str_;
};  // class ostringstream

}  // namespace rtk