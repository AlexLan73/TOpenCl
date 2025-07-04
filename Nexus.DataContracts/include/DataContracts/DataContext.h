#pragma once
#include "IDataContext.h"
#include "MemoryExchange/memory_nome.h"
#include "MemoryExchange/i_memory_data_handler.h"
#include "MemoryExchange/memory_processor.h"


class DataContext:public IDataContext
{
public:
	DataContext();
	void send(std::string s) override;
	void send(const IVectorChannel vector_channel) override;
	void send(const IValueChannel value_channel) override;

	void addTask(std::function<void()> task) override;

private:
};

