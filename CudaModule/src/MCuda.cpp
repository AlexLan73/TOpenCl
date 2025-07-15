#include "pch.h"
#include "CudaModule/MCuda.h"
#include "Core/ICore.h"
#include "Core/Core.h"
#include "DataContracts/IDataContext.h"
#include <iostream>

#include "interfaces/ModuleNameProvider.h"

cuda_module::CudaModule::CudaModule()
{

	std::cerr << "  Start CudaModule " <<  '\n';
	//auto s_ = std::make_unique<Core>();
	//auto q_ = std::make_unique<Loggers>();

  std::string name_module = "CUDA";
  ModuleNameProvider::instance().set_name(name_module);

  auto _temp = std::make_shared<TemperatureTask>(1);
  m_factory_->register_under_task(_temp);

  auto _activ = std::make_shared<ActiveCoresTask>(2);
  m_factory_->register_under_task(_activ);

  auto _core = std::make_shared<Core>(name_module, m_factory_);



}
