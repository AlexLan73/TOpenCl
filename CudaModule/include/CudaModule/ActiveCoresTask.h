﻿#pragma once
#include <random>
#include <string_view>
#include "SignalSynth.h"
#include "Core/IInject.h"
#include "Core/IUnderTask.h"
#include "Core/IUnderTask.h"
#include "interfaces/i_memory_config_channel.h"
#include "Logger/ILogger.h"

class ActiveCoresTask : public IUnderTask {
  int id_;
  Params params_;
  bool running_ = false;
  int ind_ = 0;
  SignalSynth synth; // По умолчанию: A0=1.0, A1=0.1, f=1.0
public:
  explicit ActiveCoresTask(int id) : id_(id)
  {
//    ILoggerChannel log1{ 1, "ActiveCoresTask", " Di  test!!! ", logger_send_enum_memory::warning };
//    logger_->log(log1);

  }
  int id() const override { return id_; }
//  void start() override { running_ = true; }
  void stop() override { running_ = false; }
  void pause() override { /* Реализация паузы */ }
  void set_params(const Params& p) override { params_ = p; }
  Params get(int ind = -1) const override {
    Params result = params_;
    result["cores"] = 3840; // Пример
    return result;
  }

  void start() override 
  {
    ILoggerChannel log1{ 1, "ActiveCoresTask", " Di  test!!! ", logger_send_enum_memory::warning };
    logger_->log(log1);
//    data_context_->send("ActiveCoresTask");
    running_ = true; 
  }

  void send_data() override
  {
    ind_++;
    std::string s = " " + std::to_string(ind_) + "  id= " + std::to_string(id_) + "   ActiveCoresTask";
//    data_context_->send(s);

//    std::random_device rd;  // источник энтропии
//    std::mt19937 gen(rd()); // генератор случайных чисел (Вихрь Мерсенна)
//    std::uniform_real_distribution<> dist(20.0, 90.0); // равномерное распределение от 20.0 до 90.0

    metadata_map meta_ = meta;
    meta_[static_cast<std::string>(NameTypeChannel)] = "2";

  	IIdVecValueDtChannel v;
    v.id = id_;

    // Создаём 10 элементов SValueDt, все поля = 1
    v.vec_value_dt.resize(10);
    for (auto& elem : v.vec_value_dt) 
      elem = synth.get();

    data_context_->send(2, v, meta_);
  }
};


//SignalSynth synth; // По умолчанию: A0=1.0, A1=0.1, f=1.0
//synth.set_params(2.0, 0.3, 5.0); // Изменить параметры по необходимости

/*

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