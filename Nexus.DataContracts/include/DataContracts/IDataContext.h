
#pragma once
#include <memory>
#include "interfaces/ISendLogger.h"
#include "interfaces/i_memory_config_channel.h"
//#include "Protocol.h"

class IProtocol;

class IDataContext:public ISendLogger {
public:
  virtual ~IDataContext() = default;
  virtual void send(int channel_type, const ILoggerChannel& data, const metadata_map& meta = {}) = 0;
  virtual void send(int channel_type, const IIdValueDtChannel& data, const metadata_map& meta = {}) = 0;
  virtual void send(int channel_type, const IIdVecValueDtChannel& data, const metadata_map& meta = {}) = 0;
  virtual void start() = 0;
  virtual void set_protocol(std::shared_ptr<IProtocol> protocol)=0;

};

