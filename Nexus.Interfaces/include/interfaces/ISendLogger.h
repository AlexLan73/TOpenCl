#pragma once
#include "i_memory_config_channel.h"

struct ISendLogger
{
  virtual ~ISendLogger() = default;
  virtual  void send_logger(ILoggerChannel msg) = 0;
  virtual  void send_logger(const ILoggerChannel& data, const metadata_map& meta = {}) =0;
	
};


