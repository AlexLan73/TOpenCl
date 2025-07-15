#pragma once
#include <memory>
//#include "IDataContext.h"
#include "interfaces/i_memory_config_channel.h"

class IDataContext;

class IProtocol {
public:
  virtual ~IProtocol() = default;
  virtual void process_meta_data(const metadata_map& meta_data) = 0;
  virtual void reset_counters() = 0;
  virtual void set_data_context(std::shared_ptr<IDataContext> data_context)=0;

};
