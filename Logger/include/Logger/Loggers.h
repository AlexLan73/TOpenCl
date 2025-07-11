#pragma once
#pragma execution_character_set("utf-8")
#include "ILogger.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>


class Loggers: public ILogger
{
public:
	explicit Loggers(std::string& name);
	void log(const ILoggerChannel& msg) override;
  void set_data_context(std::shared_ptr<ISendLogger> ctx) override {
    data_context_ = ctx;
  }

private:
	std::shared_ptr<spdlog::logger> logger_;
	std::string to_string(logger_send_enum_memory code);
	std::string name_logger_;
  std::shared_ptr<ISendLogger> data_context_;
  metadata_map  meta;

};

/*
// Интерфейс
struct ILogger {
  virtual void set_data_context(std::shared_ptr<IDataContext> ctx) = 0;
  // ...
};

// Реализация
class Loggers : public ILogger {
  std::shared_ptr<IDataContext> data_context_;
public:
  void set_data_context(std::shared_ptr<IDataContext> ctx) override {
    data_context_ = ctx;
  }
  // ...
};

*/