#pragma once

#include <string>
#include "interfaces/ISend.h"
#include "interfaces/ISendLogger.h"
#include "interfaces/i_memory_config_channel.h"

class ILogger:public ISendLogger
{
public:
//	log({ "CudaModule", "Опрос температуры", logger_send_enum_memory::info });  // NOLINT(clang-diagnostic-invalid-utf8)
	virtual void log(ILoggerChannel logger_channel);
	
};


