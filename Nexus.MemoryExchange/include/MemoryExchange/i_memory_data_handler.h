#pragma once
// i_memory_data_handler.h

#include "shared_data_types.h"
#include <iostream>    // ��� std::cout
#include <vector>      // ��� std::vector

// IMemoryDataHandler - ��� "��������" ��� ���������.
// ����� �����, ������� ����� ������������ ������, ������������ ����� MemoryData,
// ������ ������������ ���� ��������� � �������������� ������ ������.
class IMemoryDataHandler {
public:
  // ����������� ���������� ���������� ��� ������� ������� � ������������ ���������.
  virtual ~IMemoryDataHandler()
  {
  }

  // --- ����������� �������-�������� (stubs) ---
  // ��� ��� ����� ���������� �� ���������, ������� �� �� ����������� ��������������.

  virtual void on_ack_received() {
    std::cout << "[IMemoryDataHandler] �������� ������������� (ACK), �� ���������� �� �������������.\n";
  }

  virtual void on_logger_data(const std::vector<IdLogger>& data) {
    std::cout << "[IMemoryDataHandler] �������� ������ Logger, �� ���������� �� �������������. ����������: " << data.size() << "\n";
  }

  virtual void on_vector_data(const std::vector<IdVector>& data) {
    std::cout << "[IMemoryDataHandler] �������� ������ CudaVector, �� ���������� �� �������������. ����������: " << data.size() << "\n";
  }

  virtual void on_value_data(const std::vector<IdValue>& data) {
    std::cout << "[IMemoryDataHandler] �������� ������ CudaValue, �� ���������� �� �������������. ����������: " << data.size() << "\n";
  }

  virtual void on_date_time_variable_data(const std::vector<DateTimeVariable>& data) {
    std::cout << "[IMemoryDataHandler] �������� ������ CudaDateTimeVariable, �� ���������� �� �������������. ����������: " << data.size() << "\n";
  }

  virtual void on_matrix_data(const std::vector<IdMatrix>& data) {
    std::cout << "[IMemoryDataHandler] �������� ������ CudaMatrix, �� ���������� �� �������������. ����������: " << data.size() << "\n";
  }

  virtual void on_rec_result_data(const std::vector<RecResult>& data) {
    std::cout << "[IMemoryDataHandler] �������� ������ RecResult, �� ���������� �� �������������. ����������: " << data.size() << "\n";
  }

  virtual void on_dt_record_data(const std::vector<DtRecord>& data) {
    std::cout << "[IMemoryDataHandler] �������� ������ CudaDtRecord, �� ���������� �� �������������. ����������: " << data.size() << "\n";
  }
  // �������� ������ ����������� ������ ��� ������� ���� ������, ���� ��� ����� � ����������.


};
