
#pragma once

#include "interfaces/ISendLogger.h"
#include "interfaces/i_memory_config_channel.h"

class IDataContext:public ISendLogger {
public:
  virtual ~IDataContext() = default;
  virtual void send(int channel_type, const ILoggerChannel& data, const metadata_map& meta = {}) = 0;
  virtual void send(int channel_type, const IIdValueDtChannel& data, const metadata_map& meta = {}) = 0;
  virtual void send(int channel_type, const IIdVecValueDtChannel& data, const metadata_map& meta = {}) = 0;
};
