#include "pch.h"
#include "Core/Core.h"
#include <conio.h> 
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


  logger->set_data_context(std::static_pointer_cast<ISendLogger>(data_context));

  // 3. Используем логгер
  ILoggerChannel log1{ 1, "CudaModule", " Time max!!! ", logger_send_enum_memory::warning };
  ILoggerChannel log2{ 2, "Nexus.Core", "Start sensor", logger_send_enum_memory::info };
  ILoggerChannel log3{ 3, "Logger", "Error inicial", logger_send_enum_memory::error };

  logger->log(log1);
  logger->log(log2);
  logger->log(log3);
  
  // Предположим, factory_under_task_ — это std::shared_ptr<std::map<Key, std::shared_ptr<IUnderTask>>>
  if (factory_under_task_) {
      for (const auto& task_ptr : factory_under_task_->get_tasks() | std::views::values) {
        if (task_ptr) {
          task_ptr->start();
        }
      }
  }

  generator_ = std::make_shared<EventGenerator>(0.5);
  // Подписка:
  if (factory_under_task_) {
    for (const auto& task_ptr : factory_under_task_->get_tasks() | std::views::values) {
      if (task_ptr) {
        // Важно: task_ptr — это shared_ptr<IUnderTask>
        generator_->subscribe(task_ptr->id(), [task = task_ptr]() { task->send_data(); });
      }
    }
  }

  //// Проверка подписки:
  //if (generator.has_subscription(task_id)) {
  //  // ...
  //}

  //// Отписка:
  //generator.unsubscribe(task_id);

  // Управление:
  std::cerr << " ------  start()  ----   "  << '\n';
  generator_->start();
  std::this_thread::sleep_for(std::chrono::duration<double>(3.0));

/*
  std::cerr << " ------  pause()  ----   " << '\n';
  generator_->pause();
  std::this_thread::sleep_for(std::chrono::duration<double>(2.0));

  std::cerr << " ------  pause()  ----   " << '\n';
  generator_->resume();
  std::this_thread::sleep_for(std::chrono::duration<double>(3.0));

  std::cerr << " ------  stop()  ----   " << '\n';
  generator_->stop();
  std::this_thread::sleep_for(std::chrono::duration<double>(1.0));
*/

  std::cout << "Нажмите любую клавишу для продолжения...";
  _getch();  // Ждёт нажатия любой клавиши без необходимости Enter

  std::cerr << " ------  dispose()  ----   " << '\n';
  generator_->dispose();


  

}
/*
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
*/
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




