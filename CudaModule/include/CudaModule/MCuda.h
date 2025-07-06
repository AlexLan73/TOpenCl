#pragma once
#pragma execution_character_set("utf-8")

#include <boost/di.hpp>

#include "Core/ICore.h"
#include "Core/Core.h"
#include "Logger/ILogger.h"
#include "Logger/Loggers.h"
#include "DataContracts/DataContext.h"
#include "interfaces/i_memory_config_channel.h"
#include "Logger/Loggers.h"
#include "ActiveCoresTask.h"
#include "TemperatureTask.h"

namespace di = boost::di;

namespace cuda_module
{
	class CudaModule
	{
	public:
		CudaModule();
	private:
		std::shared_ptr<FactoryUnderTask> m_factory_ = std::make_shared<FactoryUnderTask>();
	};

}


