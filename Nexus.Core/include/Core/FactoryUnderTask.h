// ReSharper disable CppClangTidyClangDiagnosticInvalidUtf8
#pragma once
#include <map>
#include <memory>
#include <ranges>
#include <vector>
#include "IUnderTask.h"
#include "IInject.h"

class FactoryUnderTask {
public:
  // Добавить или заменить запись по ключу (id)  
  void register_under_task(std::shared_ptr<IUnderTask> i_under_task) {
    if (!i_under_task) return; // защита от nullptr
    int key = i_under_task->id();
    m_under_task[key] = i_under_task;  //std::move(i_under_task); // добавляем или заменяем

  }

  // Получить вектор всех ключей
  std::vector<int> get_keys() const {
    std::vector<int> keys;
    keys.reserve(m_under_task.size());
    for (const auto& key : m_under_task | std::views::keys) {
      keys.push_back(key);
    }
    return keys;
  }

  // Удалить запись по ключу, вернуть true если удалено, false если ключ не найден
  bool remove_by_key(int key) {
    return m_under_task.erase(key) > 0;
  }

  std::shared_ptr<IUnderTask> get(int k) const {
    if (const auto it = m_under_task.find(k); it != m_under_task.end()) {
      return it->second;
    }
    return nullptr;
  }

  // Метод для передачи инжектора всем зарегистрированным модулям
//  template<typename Injector>
  void inject_to_all_modules(const std::shared_ptr<ILogger>& logger, const std::shared_ptr<IDataContext>& data_context) {
    for (auto& module : m_under_task | std::views::values) {
      if (module) {
        module->inject(logger, data_context);
      }
    }
  }

  // get map
  const std::map<int, std::shared_ptr<IUnderTask>>& get_tasks() const {
    return m_under_task;
  }

private:
  std::map<int, std::shared_ptr<IUnderTask>> m_under_task= std::map<int, std::shared_ptr<IUnderTask>>();
};
