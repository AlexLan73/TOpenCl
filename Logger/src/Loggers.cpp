// ReSharper disable CppClangTidyClangDiagnosticInvalidUtf8
// ReSharper disable CppClangTidyClangDiagnosticCoveredSwitchDefault
#include "pch.h"
#include "Logger/Loggers.h"

#include <iostream>
#include <string>
#include <string_view>

#include "interfaces/my_constant.h"

Loggers::Loggers(std::string& name):name_logger_(name)
{
	std::cerr << "  Start LoggerS " << '\n';

  meta_ = metadata_map();
  meta_[static_cast<std::string>(NameTypeChannel)] = "0";

 // Создаём цветной логгер с именем "module_logger"
  if (!spdlog::get(name_logger_)) {
    logger_ = spdlog::stdout_color_mt(name_logger_);
  }
  else {
    logger_ = spdlog::get(name_logger_);
  }
}

void Loggers::log(const ILoggerChannel& msg)
{
  std::string level = to_string(msg.code);

  // Устанавливаем уровень лога для spdlog
  spdlog::level::level_enum spdlog_level;
  switch (msg.code) {
  case logger_send_enum_memory::error:   spdlog_level = spdlog::level::err; break;
  case logger_send_enum_memory::warning: spdlog_level = spdlog::level::warn; break;
  case logger_send_enum_memory::info:    spdlog_level = spdlog::level::info; break;
  default:                               spdlog_level = spdlog::level::info; break;
  }

  // Логируем с нужным уровнем и форматом
  logger_->log(
    spdlog_level,
    "[ID:{}][{}][{}] {}",
    msg.id, msg.module, level, msg.log
  );

  if (b_send_channel_) {
    metadata_map meta = meta_;
    data_context_->send_logger(msg, meta);
  }
//  data_context_->send_logger(msg);
}

std::string Loggers::to_string(logger_send_enum_memory code) {
  switch (code) {
  case logger_send_enum_memory::error:   return "ERROR";
  case logger_send_enum_memory::info:    return "INFO";
  case logger_send_enum_memory::warning: return "WARNING";
  default:                               return "UNKNOWN";
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/*

     metadata_map meta_ = meta;
    meta_[static_cast<std::string>(NameTypeChannel)] = "1";

    std::random_device rd;  // источник энтропии
    std::mt19937 gen(rd()); // генератор случайных чисел (Вихрь Мерсенна)
    std::uniform_real_distribution<> dist(30.0, 90.0); // равномерное распределение от 20.0 до 90.0

    IIdValueDtChannel v;

    auto v0 = SValueDt();
    v0.value = dist(gen);
    // Получаем текущее время с момента эпохи (обычно 1 января 1970)
    auto now = std::chrono::steady_clock::now();
    // время в наносекунды:
    v0.ticks = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    v.id = static_cast<uint32_t>(id_);
    v.value_dt = v0;

    const logger_send_enum_memory code =
      (v0.value > 80.0) ? logger_send_enum_memory::error :
      (v0.value > 60.0) ? logger_send_enum_memory::warning :
      logger_send_enum_memory::info;

    const int num_code = static_cast<int>(code);
    const ILoggerChannel log1{ id_, "TemperatureTask", std::to_string(v0.value), code };
    logger_->log(log1);


    data_context_->send(1, v, meta_);

 
 */


