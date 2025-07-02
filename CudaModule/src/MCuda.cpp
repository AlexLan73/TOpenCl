#include "pch.h"
#include "CudaModule/MCuda.h"

#include <iostream>

cuda_module::CudaModule::CudaModule()
{
	std::cerr << "  Start CudaModule " <<  '\n';
	auto s_ = std::make_unique<Core>();
	auto q_ = std::make_unique<Loggers>();

  // 1. Разворачиваем DI-контейнер только для CudaModule
  auto injector = di::make_injector(
    di::bind<ILogger>.to<Loggers>(),
    di::bind<INexusCore>.to<NexusCore>(),
    di::bind<IDataContracts>.to<NexusDataContracts>(),
    di::bind<IInterfaces>.to<NexusInterfaces>()
  );

  // 2. Получаем зависимости через DI
  auto logger = injector.create<std::shared_ptr<ILogger>>();
  auto nexusCore = injector.create<std::shared_ptr<INexusCore>>();
  auto dataContracts = injector.create<std::shared_ptr<IDataContracts>>();
  auto interfaces = injector.create<std::shared_ptr<IInterfaces>>();

  // 3. Пример: регистрируем задачу в ядре, логируем, отправляем данные
  nexusCore->addTask([] {
    // Опрос температуры
    IVector temp{ 0, {70.1, 71.3, 72.0} };
    // Опрос ядер
    IValue cores{ 1, 3840 };

    // Логирование
    logger->log({ "CudaModule", "Опрос температуры", logger_send_enum_memory::info });
    logger->log({ "CudaModule", "Опрос ядер", logger_send_enum_memory::info });

    // Передача данных
    dataContracts->send(temp);
    dataContracts->send(cores);
    });

  // 4. Запуск событийной схемы (например, через boost::signals2)
  nexusCore->start();

  // 5. Ожидание завершения (или интеграция с OpenCLApp)
  // ...
  nexusCore->stop();




}
