#pragma once

#include "interfaces/ISend.h"
#include "interfaces/i_memory_config_channel.h"

class ICore :public ISend 
{
public:

	virtual void addTask(std::function<void()> task) = 0;

	virtual void start()=0;
  virtual void stop()=0;

};

