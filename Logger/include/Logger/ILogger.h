#pragma once

#include <string>
#include "interfaces/ISend.h"
#include "interfaces/ISendLogger.h"
#include "interfaces/i_memory_config_channel.h"

class ILogger
{
public:
//	log({ "CudaModule", "����� �����������", logger_send_enum_memory::info });  // NOLINT(clang-diagnostic-invalid-utf8)
  virtual ~ILogger() = default;
  virtual void log(const ILoggerChannel& msg) = 0;
};


