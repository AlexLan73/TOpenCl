// ReSharper disable CppClangTidyClangDiagnosticInvalidUtf8
#include "pch.h"
#include "Logger/Loggers.h"

#include <iostream>
#include <string>


Loggers::Loggers(std::string name):name_logger_(name)
{
	std::cerr << "  Start LoggerS " << '\n';

 // Создаём цветной логгер с именем "module_logger"
  if (!spdlog::get(name_logger_)) {
    logger_ = spdlog::stdout_color_mt(name_logger_);
  }
  else {
    logger_ = spdlog::get(name_logger_);
  }
  //if (!spdlog::get("module_logger")) {
  //  logger_ = spdlog::stdout_color_mt("module_logger");
  //}
  //else {
  //  logger_ = spdlog::get("module_logger");
  //}
}

void Loggers::log(const ILoggerChannel& msg)
{
  std::string level = to_string(msg.code);
  logger_->info("[ID:{}][{}][{}] {}", msg.id, msg.module, level, msg.log);
}

std::string Loggers::to_string(logger_send_enum_memory code) {
  switch (code) {
  case logger_send_enum_memory::error:   return "ERROR";
  case logger_send_enum_memory::info:    return "INFO";
  case logger_send_enum_memory::warning: return "WARNING";
  default:                               return "UNKNOWN";
  }
}

/*
 
void Loggers::log(const ILoggerChannel& msg)
{
  std::cout << "[LOG] "
    << "ID: " << msg.id << " | "
    << "Module: " << msg.module << " | "
    << "Type: " << to_string(msg.code) << " | "
    << "Message: " << msg.log << '\n';
}

 */

//ILoggerChannel log1{ 1, "CudaModule", "Температура превышена!", logger_send_enum_memory::warning };  
//ILoggerChannel log2{ 2, "Nexus.Core", "Запуск опроса датчиков", logger_send_enum_memory::info };
//ILoggerChannel log3{ 3, "Logger", "Ошибка инициализации", logger_send_enum_memory::error };



/*

// 
//// Пример использования
//int main() {
//    ILoggerChannel log1{1, "CudaModule", "Температура превышена!", logger_send_enum_memory::warning};
//    ILoggerChannel log2{2, "Nexus.Core", "Запуск опроса датчиков", logger_send_enum_memory::info};
//    ILoggerChannel log3{3, "Logger", "Ошибка инициализации", logger_send_enum_memory::error};
//
//    print_logger_channel(log1);
//    print_logger_channel(log2);
//    print_logger_channel(log3);
//
//    return 0;
//}
// 
 */
