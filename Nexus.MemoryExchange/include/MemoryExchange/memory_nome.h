//  memory_nome.h
#pragma once

// ����������, ����� ������ ��� ���� ������  // NOLINT(clang-diagnostic-invalid-utf8)
#include "interfaces/i_memory_config_channel.h"
#include "MemoryExchange/shared_data_types.h"
#include "memory_base.h"


#include <string>
#include <memory>       // ��� std::unique_ptr
//#include <functional>   // ��� CallbackDataMetaData
//#include <iostream>     // ��� std::cout, std::cerr (���� ��� ���� � .cpp)

//class MemoryBase; // Forward declaration ��� MemoryBase

class MemoryNome {
public:
  MemoryNome(const std::string& name_memory, server_client role, callback_data_meta_data callback);
  ~MemoryNome() = default;

  MemoryNome(const MemoryNome&) = delete;
  MemoryNome& operator=(const MemoryNome&) = delete;

  void write_data_to_memory(const std::vector<uint8_t>& bytes, const metadata_map& map);

  // ��������� ���������� �� ������, � ������� �� �����.
      // ��������� ������, �������� �� ���������� ���� ������.
  metadata_map check_write_channel_control();

  // ������� ���������� �� ������, �� �������� �� ������.
  // ��� ����� ������ ����� ����, ��� �� ���������� �������� ������.
  void clear_read_channel_control();
private:
  std::unique_ptr<MemoryBase> memory_read_; // ���������� unique_ptr
  std::unique_ptr<MemoryBase> memory_write_; // ���������� unique_ptr
};



