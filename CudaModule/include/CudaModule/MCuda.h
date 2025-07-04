#pragma once
#pragma execution_character_set("utf-8")

#include <boost/di.hpp>

#include "Core/ICore.h"
#include "Core/Core.h"
#include "Logger/ILogger.h"
#include "Logger/Loggers.h"
#include "DataContracts/DataContext.h"
#include "interfaces/i_memory_config_channel.h"
namespace di = boost::di;

namespace cuda_module
{
	class CudaModule
	{
	public:
		CudaModule();
	private:

	};

}


