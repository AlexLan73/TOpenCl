#pragma once

#include <string>
#include "interfaces/ISendLogger.h"
#include "interfaces/ISend.h"
#include "interfaces/i_memory_config_channel.h"

class ICore :public ISend 
{
public:
	//	log({ "CudaModule", "Опрос температуры", logger_send_enum_memory::info });
//	virtual void log(ILoggerChannel logger_channel);

	virtual void addTask(std::function<void()> task) = 0;

	virtual void start()=0;
  virtual void stop()=0;

};

