// ReSharper disable CppClangTidyClangDiagnosticInvalidUtf8
#pragma once
#include <random>
#include <cmath>
#include <chrono>
#include "interfaces/i_memory_config_channel.h"
#include "interfaces/my_constant.h"

//struct SValueDt {
//  double value;
//  int64_t ticks; // .NET ticks (100 нс с 01.01.0001)  
//};

class SignalSynth {
public:
  SignalSynth(const double a0 = 1.0, const double a1 = 0.1, const double f = 1.0, const unsigned seed = 42)
    : a0_(a0), a1_(a1), f_(f), i_(0), gen_(seed), norm_dist_(0.0, 1.0)
  {
    update_w();
  }

  // Задать параметры синусоиды и шума
  void set_params(const double a0, const double a1, const double f) {
    a0_ = a0;
    a1_ = a1;
    f_ = f;
    update_w();
  }

  // Получить очередное значение сигнала и время в тиках
  SValueDt get() {
	  const double y0 = a0_ * std::sin(w_ * i_);
	  const double y1 = a1_ * norm_dist_(gen_);
    double y = y0 + y1;
    SValueDt v;
    v.value = y;
    v.ticks = get_dotnet_ticks();
    ++i_;
    return v;
  }

  // Сбросить счетчик
  void reset() { i_ = 0; }

private:
  double a0_, a1_, f_, w_;
  int64_t i_;
  std::mt19937 gen_;
  std::normal_distribution<double> norm_dist_;
  //const double pi_ = 3.141592654;
  void update_w() { w_ = 2.0 * PI * f_ / 10.0; }

  // Получить .NET ticks (100 нс с 01.01.0001)
  static int64_t get_dotnet_ticks() {
    using namespace std::chrono;
    constexpr int64_t TICKS_AT_UNIX_EPOCH = 621355968000000000LL;
    const auto now = system_clock::now();
    const auto duration = now.time_since_epoch();
    const int64_t ns = duration_cast<nanoseconds>(duration).count();
    return TICKS_AT_UNIX_EPOCH + ns / 100;
  }
};

