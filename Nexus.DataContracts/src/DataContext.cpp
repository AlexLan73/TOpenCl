#include "pch.h"
#include "DataContracts/DataContext.h"

#include <iostream>


DataContext::DataContext()
{
	std::cerr << "  Start DataContext " << '\n';
	//const std::string& name, server_client role, IMemoryDataHandler* handler
//	auto memory_ = std::make_unique<MemoryProcessor>();

}

void DataContext::send(std::string s)
{
//	IDataContext::send(s);
	std::cerr << "  DataContext -->>  " << s << '\n';

}

void DataContext::send(const IVectorChannel vector_channel)
{
	IDataContext::send(vector_channel);
}

void DataContext::send(const IValueChannel value_channel)
{
	IDataContext::send(value_channel);
}

void DataContext::addTask(std::function<void()> task)
{
}

void DataContext::send_logger(ILoggerChannel msg)
{
	std::cerr << "  FROM LOGGER  ->  "<< msg.log << '\n';
}
