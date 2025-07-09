#pragma once
// i_memory_data_handler.h

#include "shared_data_types.h"
#include <iostream>    // Для std::cout
#include <vector>      // Для std::vector

// IMemoryDataHandler - это "контракт" или интерфейс.
// Любой класс, который хочет обрабатывать данные, передаваемые через MemoryData,
// должен унаследовать этот интерфейс и переопределить нужные методы.
class IMemoryDataHandler {
public:
  // Виртуальный деструктор ОБЯЗАТЕЛЕН для базовых классов с виртуальными функциями.
  virtual ~IMemoryDataHandler()
  {
  }

  // --- Виртуальные функции-заглушки (stubs) ---
  // Они уже имеют реализацию по умолчанию, поэтому их не обязательно переопределять.

  virtual void on_ack_received() {
    std::cout << "[IMemoryDataHandler] Получено подтверждение (ACK), но обработчик не переопределен.\n";
  }

  virtual void on_logger_data(const std::vector<IdLogger>& data) {
    std::cout << "[IMemoryDataHandler] Получены данные Logger, но обработчик не переопределен. Количество: " << data.size() << "\n";
  }

  // Добавьте другие виртуальные методы для каждого типа данных, если они нужны в интерфейсе.



};
