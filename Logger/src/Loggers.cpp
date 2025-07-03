// ReSharper disable CppClangTidyClangDiagnosticInvalidUtf8
#include "pch.h"
#include "Logger/Loggers.h"

#include <iostream>
#include <string>


Loggers::Loggers()
{
	std::cerr << "  Start LoggerS " << '\n';
}

void Loggers::send(const std::string s)
{
	std::cerr << " Logger -- "<< s << '\n';

}

void Loggers::log(ILoggerChannel logger_channel)
{
  std::cout << "[LOG] "
    << "ID: " << logger_channel.id << " | "
    << "Module: " << logger_channel.module << " | "
    << "Type: " << to_string(logger_channel.code) << " | "
    << "Message: " << logger_channel.log << '\n';
}
std::string Loggers::to_string(logger_send_enum_memory code) {
  switch (code) {
  case logger_send_enum_memory::error:   return "ERROR";
  case logger_send_enum_memory::info:    return "INFO";
  case logger_send_enum_memory::warning: return "WARNING";
  default:                               return "UNKNOWN";
  }
}


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