#pragma once
#include <functional>
#include <iostream>
#include <string>
#include "i_memory_config_channel.h"

class ISend
{
public:
	virtual ~ISend() = default;

	virtual void send(const IVectorChannel vector_channel)
	{
		std::cerr << "  ISend -> IVectorChannel " <<  '\n';
	}
	virtual void send(const IValueChannel value_channel)
	{
		std::cerr << "  ISend -> IVectorChannel " <<  '\n';
	}

	virtual void addTask(std::function<void()> task) = 0;
};


