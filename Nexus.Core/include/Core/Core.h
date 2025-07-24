#pragma once
#pragma execution_character_set("utf-8")
#include <vector>
#include <mutex>

#include "EventGenerator.h"
#include "ICore.h"
#include "interfaces/TimeCounters.h"
#include "FactoryUnderTask.h"
#include "Logger/Loggers.h"

class Core final :public ICore
{
public:
	Core(std::string name_module, std::shared_ptr<FactoryUnderTask> factory_under_task_);
  void add_count_time();
  void compare_count_time();

  void start() override;
	void stop() override;
	void addTask(std::function<void()> task) override;

private:
	std::vector<std::function<void()>> tasks_;
	std::mutex mutex_;
	std::string name_module_ = "";
	std::shared_ptr<EventGenerator> generator_;
	std::shared_ptr<EventGenerator> generator_0_5sec_;
	std::shared_ptr<ServerMetaDataTimer> md_time_;
	std::shared_ptr<ILogger> i_logger_;
	std::shared_ptr<IDataContext> i_data_context_;
	std::shared_ptr<IProtocol> i_protocol_;

};

