#include "pch.h"
#include "Core/Core.h"

#include <iostream>
#include <mutex>

Core::Core()
{
	std::cerr << "  Start Core " << '\n';
	auto _data_context = std::make_shared<DataContext>();
}

void Core::send(std::string s)
{
//	ICore::send(s);
	std::cerr << "  Core send -->> " << s << '\n';

}

void Core::send(const IVectorChannel vector_channel)
{
	std::cerr << "  Core send -->> IVectorChannel"  << '\n';
	ICore::send(vector_channel);
}

void Core::send(const IValueChannel value_channel)
{
	std::cerr << "  Core send -->> IValueChannel" << '\n';
	ICore::send(value_channel);
}

void Core::start()
{
	std::cerr << "  Core start "  << '\n';

	std::lock_guard<std::mutex> lock(mutex_);
	for (auto& task : tasks_) {
			if (task) task();
	}
	
}

void Core::stop()
{
	std::cerr << "  Core stop -->> " << '\n';

}

void Core::addTask(std::function<void()> task)
{
		std::lock_guard<std::mutex> lock(mutex_);
		tasks_.push_back(std::move(task));
}




