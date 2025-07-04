#include "pch.h"
#include "Core/Core.h"

#include <iostream>
#include <mutex>
#include <utility>

Core::Core(std::string name_module, std::shared_ptr<FactoryUnderTask> factory_under_task_)
		:name_module_(std::move(name_module))
{
	std::cerr << "  Start Core " << '\n';
	//auto _data_context = std::make_shared<DataContext>();

  // 1. Разворачиваем DI-контейнер только для CudaModule
  
  auto injector = di::make_injector(
    di::bind<ILogger>.to<Loggers>(),
    di::bind<std::string>.named<logger_tag>().to(name_module_),
    di::bind<IDataContext>.to<DataContext>()
  );

  // 2. Получаем зависимости через DI
  auto logger = injector.create<std::shared_ptr<ILogger>>();
  auto data_context = injector.create<std::shared_ptr<IDataContext>>();


  factory_under_task_->inject_to_all_modules(injector);

  // 3. Используем логгер
  ILoggerChannel log1{ 1, "CudaModule", " Time max!!! ", logger_send_enum_memory::warning };
  ILoggerChannel log2{ 2, "Nexus.Core", "Start sensor", logger_send_enum_memory::info };
  ILoggerChannel log3{ 3, "Logger", "Error inicial", logger_send_enum_memory::error };

  logger->log(log1);
  logger->log(log2);
  logger->log(log3);
  
  // Предположим, factory_under_task_ — это std::shared_ptr<std::map<Key, std::shared_ptr<IUnderTask>>>
  //if (factory_under_task_) {
  //  for (auto& [key, task_ptr] : *factory_under_task_) {
  //    if (task_ptr) {
  //      task_ptr->test_di();
  //    }
  //  }
  //}


  // 3. Пример: регистрируем задачу в ядре, логируем, отправляем данные
  //nexusCore->addTask([logger, data_context] {
  //  IVectorChannel temp{ 0, {70.1, 71.3, 72.0} };
  //  IValueChannel cores{ 1, 3840 };

  //  logger->log({ 0,"CudaModule", "Опрос температуры", logger_send_enum_memory::info });
  //  logger->log({ 0,"CudaModule", "Опрос ядер", logger_send_enum_memory::info });

  //  data_context->send(temp);
  //  data_context->send(cores);
  //  });




  //// 4. Запуск событийной схемы (например, через boost::signals2)
  //nexusCore->start();

  //// 5. Ожидание завершения (или интеграция с OpenCLApp)
  //// ...
  //nexusCore->stop();


























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




