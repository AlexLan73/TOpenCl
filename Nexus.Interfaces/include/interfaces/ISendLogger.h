#pragma once
#include <functional>
#include <iostream>
#include <string>
#include "i_memory_config_channel.h"

class ISendLogger
{
public:
	virtual ~ISendLogger() = default;
	virtual void send(const std::string s)
	{
		std::cerr << "  ISend -> "<< s << '\n';
	}
};


