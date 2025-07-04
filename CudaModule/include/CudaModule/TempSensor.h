#pragma once
#pragma once
#include "Logger/ILogger.h"
#include "interfaces/i_memory_config_channel.h"
#include "DataContracts/DataContext.h"
#include "DataContracts/IDataContext.h"


class TempSensor {
public:
  TempSensor(std::shared_ptr<ILogger> logger, std::shared_ptr<IDataContext> data_context)
    : logger_(std::move(logger)), dataContext_(std::move(data_context)) {
  }

  void poll() {
    IVectorChannel temp{ 0, {getTemperature()} };
    logger_->log({ "TempSensor", "Температура: " + std::to_string(temp.values[0]), logger_send_enum_memory::info });
    dataContext_->send(temp);
  }
private:
  float getTemperature() { /* Реальный опрос CUDA */ return 42.5f; }
  std::shared_ptr<ILogger> logger_;
  std::shared_ptr<IDataContext> dataContext_;
};
