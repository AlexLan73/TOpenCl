#pragma once
#include <iostream>
#include <chrono>
#include <cstdint>

#include "Core/IInject.h"
#include "Core/IUnderTask.h"
#include "Logger/ILogger.h"

class TemperatureTask : public IUnderTask {
  int id_;
  Params params_;
  bool running_ = false;
  int ind1_ = 0;
//  std::random_device rd;  // источник энтропии
//  std::mt19937 gen(rd()); // генератор случайных чисел (Вихрь Мерсенна)
//  std::uniform_real_distribution<> dist;

public:
  TemperatureTask(int id) : id_(id)
  {
//    std::random_device rd;  // источник энтропии
//    std::mt19937 gen(rd()); // генератор случайных чисел (Вихрь Мерсенна)
//    dist = std::uniform_real_distribution<>(20.0, 90.0); // равномерное распределение от 20.0 до 90.0

  }
  ~TemperatureTask() {};
  int id() const override { return id_; }
//  void start() override { running_ = true;     /* Запуск измерения */ }//
  void stop() override { running_ = false; }
  void pause() override {  }                  /* Реализация паузы */
  void set_params(const Params& p) override { params_ = p; }
  Params get(int ind = -1) const override {
    Params result = params_;
    result["temperature"] = std::vector<double>{ 70.1, 71.3, 72.0 }; // Пример
    return result;
  }

  void start() override
  {
    ILoggerChannel log1{ 1, "TemperatureTask", " Di  test!!! ", logger_send_enum_memory::warning };
    logger_->log(log1);
//    data_context_->send("TemperatureTask");

    running_ = true;
  }

  void send_data() override
  {
    ind1_++;
    std::string s = " " + std::to_string(ind1_) + "  id= " + std::to_string(id_) + "   TemperatureTask";

    ILoggerChannel log1{ 1, "TemperatureTask", " Di  test!!! ", logger_send_enum_memory::warning };
    logger_->log(log1);
    metadata_map meta_ = meta;
    meta_[static_cast<std::string>(NameTypeChannel)] = "1";

  	std::random_device rd;  // источник энтропии
    std::mt19937 gen(rd()); // генератор случайных чисел (Вихрь Мерсенна)
    std::uniform_real_distribution<> dist(20.0, 90.0); // равномерное распределение от 20.0 до 90.0

    IIdValueDtChannel v;

    auto v0 = SValueDt();
    v0.value = dist(gen);
    // Получаем текущее время с момента эпохи (обычно 1 января 1970)
    auto now = std::chrono::steady_clock::now();
    // время в наносекунды:
    v0.ticks = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    v.id = static_cast<uint32_t>(id_);
    v.value_dt = v0;
    data_context_->send(1, v, meta_);
  }
private:

};


/*

    std::vector<IIdValueDtChannel> v(10);
    for (auto& [id, value_dt] : v) {
      auto v0 = SValueDt();
      v0.value= dist(gen);
      // Получаем текущее время с момента эпохи (обычно 1 января 1970)
      auto now = std::chrono::steady_clock::now();
      // время в наносекунды:
      v0.ticks = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
      value_dt = v0;
      id = static_cast<uint32_t>(id_);

      }


 struct IIdValueDtChannel {
  uint32_t id;
  SValueDt value_dt;
};

// Канал вектора измерений
// e_id_vec_value_dt_channel = 2,
struct IIdVecValueDtChannel {
  uint32_t id;
  std::vector<SValueDt> vec_value_dt;
};

 
 */
