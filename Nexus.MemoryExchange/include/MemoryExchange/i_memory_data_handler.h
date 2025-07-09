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

/*
  virtual void on_vector_data(const std::vector<IdVector>& data) {
    std::cout << "[IMemoryDataHandler] Получены данные CudaVector, но обработчик не переопределен. Количество: " << data.size() << "\n";
  }

  virtual void on_value_data(const std::vector<IdValue>& data) {
    std::cout << "[IMemoryDataHandler] Получены данные CudaValue, но обработчик не переопределен. Количество: " << data.size() << "\n";
  }

  virtual void on_date_time_variable_data(const std::vector<DateTimeVariable>& data) {
    std::cout << "[IMemoryDataHandler] Получены данные CudaDateTimeVariable, но обработчик не переопределен. Количество: " << data.size() << "\n";
  }

  virtual void on_matrix_data(const std::vector<IdMatrix>& data) {
    std::cout << "[IMemoryDataHandler] Получены данные CudaMatrix, но обработчик не переопределен. Количество: " << data.size() << "\n";
  }

  virtual void on_rec_result_data(const std::vector<RecResult>& data) {
    std::cout << "[IMemoryDataHandler] Получены данные RecResult, но обработчик не переопределен. Количество: " << data.size() << "\n";
  }

  virtual void on_dt_record_data(const std::vector<DtRecord>& data) {
    std::cout << "[IMemoryDataHandler] Получены данные CudaDtRecord, но обработчик не переопределен. Количество: " << data.size() << "\n";
  }
*/
  // Добавьте другие виртуальные методы для каждого типа данных, если они нужны в интерфейсе.


};
