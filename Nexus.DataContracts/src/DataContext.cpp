#include "pch.h"
#include "DataContracts/DataContext.h"

#include <iostream>


DataContext::DataContext()
{
	std::cerr << "  Start DataContext " << '\n';
	//const std::string& name, server_client role, IMemoryDataHandler* handler
//	auto memory_ = std::make_unique<MemoryProcessor>();

}
