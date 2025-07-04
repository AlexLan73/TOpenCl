#pragma once
#include "Core/IInject.h"
#include "Core/IUnderTask.h"
#include "Core/IUnderTask.h"
#include "interfaces/i_memory_config_channel.h"
#include "Logger/ILogger.h"

class ActiveCoresTask : public IUnderTask {
  int id_;
  Params params_;
  bool running_ = false;
public:
  ActiveCoresTask(int id) : id_(id)
  {
    ILoggerChannel log1{ 1, "ActiveCoresTask", " Di  test!!! ", logger_send_enum_memory::warning };
    logger_->log(log1);

  }
  int id() const override { return id_; }
  void start() override { running_ = true; }
  void stop() override { running_ = false; }
  void pause() override { /* Реализация паузы */ }
  void set_params(const Params& p) override { params_ = p; }
  Params get(int ind = -1) const override {
    Params result = params_;
    result["cores"] = 3840; // Пример
    return result;
  }

  void test_di()
  {
    ILoggerChannel log1{ 1, "ActiveCoresTask", " Di  test!!! ", logger_send_enum_memory::warning };
    logger_->log(log1);
  }


};

