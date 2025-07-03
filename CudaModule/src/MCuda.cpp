#include "pch.h"
#include "CudaModule/MCuda.h"
#include "Core/ICore.h"
#include "DataContracts/IDataContext.h"
#include <iostream>

cuda_module::CudaModule::CudaModule()
{
	std::cerr << "  Start CudaModule " <<  '\n';
	auto s_ = std::make_unique<Core>();
	auto q_ = std::make_unique<Loggers>();

  // 1. ������������� DI-��������� ������ ��� CudaModule
  auto injector = di::make_injector(
    di::bind<ILogger>.to<Loggers>(),
    di::bind<ICore>.to<Core>(),
    di::bind<IDataContext>.to<DataContext>()
  );

  // 2. �������� ����������� ����� DI
  auto logger = injector.create<std::shared_ptr<ILogger>>();
  auto nexusCore = injector.create<std::shared_ptr<ICore>>();
  auto data_context = injector.create<std::shared_ptr<IDataContext>>();
  
  // 3. ������: ������������ ������ � ����, ��������, ���������� ������
  nexusCore->addTask([logger, data_context] {
    IVectorChannel temp{ 0, {70.1, 71.3, 72.0} };
    IValueChannel cores{ 1, 3840 };

    logger->log({ 0,"CudaModule", "����� �����������", logger_send_enum_memory::info });
    logger->log({ 0,"CudaModule", "����� ����", logger_send_enum_memory::info });

    data_context->send(temp);
    data_context->send(cores);
    });




  // 4. ������ ���������� ����� (��������, ����� boost::signals2)
  nexusCore->start();

  // 5. �������� ���������� (��� ���������� � OpenCLApp)
  // ...
  nexusCore->stop();

}
