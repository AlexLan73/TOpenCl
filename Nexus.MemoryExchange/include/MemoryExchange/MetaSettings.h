// ReSharper disable CppClangTidyClangDiagnosticInvalidUtf8
#pragma once

#include <string>
#include <stdexcept>

class MetaSettings {
public:
  // Конструктор с параметрами, проверка аргументов выполнена
  explicit MetaSettings(std::string name, int metaSize = 8192)
    : MemoryName(std::move(name)), MetaSize(metaSize)
  {
    if (MemoryName.empty()) {
      throw std::invalid_argument("name cannot be empty");
    }
    if (MetaSize <= 0) {
      throw std::out_of_range("metaSize must be positive");
    }
  }

  // Только геттеры для полей (по аналогии с C# public string MemoryName { get; })
  [[nodiscard]] const std::string& GetMemoryName() const { return MemoryName; }
  [[nodiscard]] int GetMetaSize() const { return MetaSize; }

  // Свойства, возвращающие сформированные строки
  [[nodiscard]] std::string GetMetaEventServer() const {
    return "Global\\EventServer" + MemoryName;
  }

  [[nodiscard]] std::string GetMetaEventClient() const {
    return "Global\\EventClient" + MemoryName;
  }

  [[nodiscard]] std::string GetControlName() const {
    return MemoryName + "Control";
  }

private:
  std::string MemoryName;
  int MetaSize;
};
