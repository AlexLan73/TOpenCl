#pragma once
#pragma execution_character_set("utf-8")
#include <vector>
#include <mutex>

#include <boost/di.hpp>

#include "EventGenerator.h"
#include "ICore.h"

#include "FactoryUnderTask.h"

namespace di = boost::di;

class Core final :public ICore
{
public:
	Core(std::string name_module, std::shared_ptr<FactoryUnderTask> factory_under_task_);

	void start() override;
	void stop() override;
	void addTask(std::function<void()> task) override;

private:
	std::vector<std::function<void()>> tasks_;
	std::mutex mutex_;
	std::string name_module_ = "";
	std::shared_ptr<EventGenerator> generator_;
};

