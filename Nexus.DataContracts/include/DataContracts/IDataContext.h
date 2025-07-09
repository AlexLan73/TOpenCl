#pragma once

#include <string>
#include "interfaces/ISendLogger.h"
#include "interfaces/ISend.h"
#include "interfaces/i_memory_config_channel.h"
#include "interfaces/ISendLogger.h"
#include "MemoryExchange/i_memory_data_handler.h"

class IDataContext : public ISendLogger //, public IChannelTypeData
{
public:
  
  
  virtual  void send(int channel_type, const ILoggerChannel& data, const metadata_map& meta = {}) = 0;
  virtual  void send(int channel_type, const IIdValueDtChannel& data, const metadata_map& meta = {}) = 0;
  virtual  void send(int channel_type, const IIdVecValueDtChannel& data, const metadata_map& meta = {}) = 0;

};

