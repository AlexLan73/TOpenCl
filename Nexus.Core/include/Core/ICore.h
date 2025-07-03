#pragma once

#include <string>
#include "interfaces/ISendLogger.h"
#include "interfaces/ISend.h"
#include "interfaces/i_memory_config_channel.h"

class ICore :public ISend, ISendLogger
{
public:
	//	log({ "CudaModule", "Опрос температуры", logger_send_enum_memory::info });
//	virtual void log(ILoggerChannel logger_channel);

  void addTask(std::function<void()> task) override;

	virtual void start()=0;
  virtual void stop()=0;

};

