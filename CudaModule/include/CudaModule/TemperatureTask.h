#pragma once
#include "Core/IInject.h"
#include "Core/IUnderTask.h"
#include "Logger/ILogger.h"

class TemperatureTask : public IUnderTask {
  int id_;
  Params params_;
  bool running_ = false;
public:
  TemperatureTask(int id) : id_(id)
  {
  }
  ~TemperatureTask() {};
  int id() const override { return id_; }
  void start() override { running_ = true;     /* Запуск измерения */ }
  void stop() override { running_ = false; }
  void pause() override {  }                  /* Реализация паузы */
  void set_params(const Params& p) override { params_ = p; }
  Params get(int ind = -1) const override {
    Params result = params_;
    result["temperature"] = std::vector<double>{ 70.1, 71.3, 72.0 }; // Пример
    return result;
  }

  void test_di()
  {
    ILoggerChannel log1{ 1, "TemperatureTask", " Di  test!!! ", logger_send_enum_memory::warning };
    logger_->log(log1);
  }
private:


};


