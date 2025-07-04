#pragma once
#pragma execution_character_set("utf-8")
#include <vector>
#include <mutex>

#include <boost/di.hpp>

#include "ICore.h"
#include "Logger/ILogger.h"
#include "Logger/Loggers.h"
#include "DataContracts/DataContext.h"
#include "interfaces/i_memory_config_channel.h"

#include "FactoryUnderTask.h"

namespace di = boost::di;

class Core:public ICore
{
public:
	Core(std::string name_module, std::shared_ptr<FactoryUnderTask> factory_under_task_);
	void send(std::string s) override;
	void send(const IVectorChannel vector_channel) override;
	void send(const IValueChannel value_channel) override;


	void start() override;
	void stop() override;
	void addTask(std::function<void()> task) override;

private:
	std::vector<std::function<void()>> tasks_;
	std::mutex mutex_;
	std::string name_module_ = "";
};

