#pragma once

#include <vector>
#include <mutex>


#include "DataContracts/DataContext.h"

#include "ICore.h"

class Core:public ICore
{
public:
	Core();
	void send(std::string s) override;
	void send(const IVectorChannel vector_channel) override;
	void send(const IValueChannel value_channel) override;


	void start() override;
	void stop() override;
	void addTask(std::function<void()> task) override;

private:
	std::vector<std::function<void()>> tasks_;
	std::mutex mutex_;

};

