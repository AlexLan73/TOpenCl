#pragma once

#include <string>
#include "interfaces/ISendLogger.h"
#include "interfaces/ISend.h"
#include "interfaces/i_memory_config_channel.h"

class IDataContext :public ISend
{
public:
	//	log({ "CudaModule", "Опрос температуры", logger_send_enum_memory::info });
//	virtual void log(ILoggerChannel logger_channel);

};

