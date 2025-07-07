#include "core/logging.h"
#include <cstdarg>
#include "core/format.h"
#include "core/stdlib/cstring.h"
using namespace rtk;

// Printf-style formatter outputting to an ostream
class OstreamFormatter : public Formatter {
 public:
  OstreamFormatter(rtk::ostream& stream) : stream_{stream} {}

 protected:
  // just outputs to an ostream
  size_t outputChars(const char* chars) const override {
    auto chrs = rtk::strlen(chars);
    stream_.write(chars);
    //charsPrinted_ += chrs;
    return chrs;
  }

 private:
  rtk::ostream& stream_;
};

// This is where we do printf-style logging!
// Logger uses Formatter to do the dirty work, we
// pass in our ostream.
void rtk::Logger::vlog(const rtk::string_view format, va_list argp) const {
  OstreamFormatter formatter{stream()};
  formatter.vparsef(format.c_str(), argp);
}