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
private:
	std::shared_ptr<spdlog::logger> logger_;
	std::string to_string(logger_send_enum_memory code);
	std::string name_logger_ = "";
};

