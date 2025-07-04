// ReSharper disable CppClangTidyClangDiagnosticInvalidUtf8
#include "pch.h"
#include "Logger/Loggers.h"

#include <iostream>
#include <string>


Loggers::Loggers()
{
	std::cerr << "  Start LoggerS " << '\n';

 // ������ ������� ������ � ������ "module_logger"
 if (!spdlog::get("module_logger")) {
	logger_ = spdlog::stdout_color_mt("module_logger");
 }
 else {
	logger_ = spdlog::get("module_logger");
 }
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

//ILoggerChannel log1{ 1, "CudaModule", "����������� ���������!", logger_send_enum_memory::warning };  
//ILoggerChannel log2{ 2, "Nexus.Core", "������ ������ ��������", logger_send_enum_memory::info };
//ILoggerChannel log3{ 3, "Logger", "������ �������������", logger_send_enum_memory::error };



/*

// 
//// ������ �������������
//int main() {
//    ILoggerChannel log1{1, "CudaModule", "����������� ���������!", logger_send_enum_memory::warning};
//    ILoggerChannel log2{2, "Nexus.Core", "������ ������ ��������", logger_send_enum_memory::info};
//    ILoggerChannel log3{3, "Logger", "������ �������������", logger_send_enum_memory::error};
//
//    print_logger_channel(log1);
//    print_logger_channel(log2);
//    print_logger_channel(log3);
//
//    return 0;
//}
// 
 */