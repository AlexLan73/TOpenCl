#pragma once
#include <functional>
#include <iostream>
#include <string>
#include "i_memory_config_channel.h"

struct ISendLogger
{
//	virtual ~ISendLogger() = default;
	virtual  void send_logger(ILoggerChannel msg) = 0;
	
};


