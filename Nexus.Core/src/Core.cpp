#include "pch.h"
#include "Core/Core.h"
#include <conio.h> 
#include <iostream>
#include <mutex>
#include <utility>

#include "DataContracts/DataContext.h"
#include "Logger/Loggers.h"
#include "DataContracts/Protocol.h"

Core::Core(std::string name_module, std::shared_ptr<FactoryUnderTask> factory_under_task_)
		:name_module_(std::move(name_module))
{
	std::cerr << "  Start Core " << '\n';
  generator_0_5sec_ = std::make_shared<EventGenerator>(0.5);

  generator_0_5sec_->subscribe(0, [this]() { Core::add_count_time(); });
  generator_0_5sec_->subscribe(0, [this]() { Core::compare_count_time(); });

  i_logger_ = std::make_shared<Loggers>(name_module_);
  i_data_context_ = std::make_shared<DataContext>(i_logger_);

  time_counters_ = std::make_shared<TimeCounters>();
  time_counters_->reset();

  i_protocol_ = std::make_shared<Protocol>(time_counters_, i_logger_);

	i_protocol_->set_data_context(i_data_context_);
  factory_under_task_->inject_to_all_modules(i_logger_, i_data_context_);
  i_logger_->set_data_context(std::static_pointer_cast<ISendLogger>(i_data_context_));

  i_data_context_->start();

  // 3. Используем логгер
  ILoggerChannel log1{ 1, "CudaModule", " Time max!!! ", logger_send_enum_memory::warning };
  ILoggerChannel log2{ 2, "Nexus.Core", "Start sensor", logger_send_enum_memory::info };
  ILoggerChannel log3{ 3, "Logger", "Error inicial", logger_send_enum_memory::error };

  i_logger_->log(log1);
  i_logger_->log(log2);
  i_logger_->log(log3);
  
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

  i_logger_->start_send_channel();
  generator_->set_interval(0.1);
  std::this_thread::sleep_for(std::chrono::duration<double>(2.0));
  generator_->set_interval(0.35);
  std::this_thread::sleep_for(std::chrono::duration<double>(1.0));
  generator_->set_interval(1.0);
  std::this_thread::sleep_for(std::chrono::duration<double>(1.0));
  generator_->set_interval(0.1);

//  std::cout << "Нажмите любую клавишу для продолжения...";
  _getch();  // Ждёт нажатия любой клавиши без необходимости Enter

  std::cerr << " ------  dispose()  ----   " << '\n';
  generator_->dispose();
}

void Core:: add_count_time(){
  time_counters_->event_timer();
}
void Core::compare_count_time() {
  
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




