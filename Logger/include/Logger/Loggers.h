#pragma once
#include "ILogger.h"

class Loggers: public ILogger
{
public:
	Loggers();
	void send(const std::string s) override;
	void log(ILoggerChannel logger_channel) override;
private:
	std::string to_string(logger_send_enum_memory code);
};

