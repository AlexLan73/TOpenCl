#pragma once
#include <functional>

class ISend
{
public:
	virtual ~ISend() = default;

	virtual void addTask(std::function<void()> task) = 0;
};


