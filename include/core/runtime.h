#pragma once

#include "core/di.h"
#include "core/logging.h"
#include "core/status.h"
#include "core/stdlib/memory.h"

namespace rtk {
// Application-supplied
StatusCode CreateServices();

// TODO: inject logger, allocator, etc.
// Call in CreateServices
StatusCode InitializeGlobalContext(Logger& logger);

// Built-in; call before or in main function
StatusCode CoreRuntimeMain();

class RuntimeServiceContext final : public ServiceContext {
 public:
  RuntimeServiceContext(unique_ptr<Logger> logger) : logger_{move(logger)} {}

  Logger& logger() { return *logger_; }
  const Logger& logger() const { return *logger_; }
  void setLogger(unique_ptr<Logger> logger) { logger_ = move(logger); }

 private:
  unique_ptr<Logger> logger_;
};  // class RuntimeServiceContext

RuntimeServiceContext& GlobalContext();

}  // namespace rtk
