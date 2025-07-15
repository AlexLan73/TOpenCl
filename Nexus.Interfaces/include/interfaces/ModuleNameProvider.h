#pragma once

#include <string>

class ModuleNameProvider {
public:
  // Получить экземпляр singleton
  static ModuleNameProvider& instance() {
    static ModuleNameProvider instance_;
    return instance_;
  }

  // Установка имени модуля (можно только один раз во время инициализации!)
  void set_name(const std::string& name) {
    name_module_ = name;
  }

  // Получить имя модуля
  const std::string& get_name() const {
    return name_module_;
  }

private:
  ModuleNameProvider() = default;
  ModuleNameProvider(const ModuleNameProvider&) = delete;
  ModuleNameProvider& operator=(const ModuleNameProvider&) = delete;

  std::string name_module_;
};