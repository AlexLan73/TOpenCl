#pragma once
//#pragma once
//// MemoryProcessor.h


#include "interfaces/i_memory_config_channel.h"  
#include "shared_data_types.h"
#include "i_memory_data_handler.h"  // ���������� ���������
#include "memory_nome.h"
//
//#include <memory>             // ��� std::unique_ptr
//#include <string>             // ��� std::string
//#include <sstream>            // ��� std::stringstream
//#include <vector>             // ��� std::vector

// ���� ����� ������������� ��� ������ ������, �������������� � ���������������.
class MemoryProcessor {
public:
  // ����������� ��������� ��������� �� ���������� ����������.
  MemoryProcessor(const std::string& name, server_client role, IMemoryDataHandler* handler);


  // ���������� ����� ��� �������� ������ (�������� ������ ������������)
  template<typename T>
  void send_data(data_type_ids type_id, const std::vector<T>& data);

  metadata_map check_write_channel();
  void clear_read_channel();

private:
  // ���������� �����, ������� ���������� MemoryNome ��� ��������� ����� ������
  void on_raw_data_received(const rec_data_meta_data& dMetaData);

  std::unique_ptr<MemoryNome> memory_nome_;
  IMemoryDataHandler* handler_; // ��������� �� ������� ����������
};

// ���������� ���������� ������ ����� �������� � ������������ �����
template<typename T>
void MemoryProcessor::send_data(const data_type_ids type_id, const std::vector<T>& data) {
  if (data.empty() || !memory_nome_) return;

  std::stringstream buffer;
  msgpack::pack(buffer, data); // ������������
  std::string packed_str = buffer.str();
  const std::vector<uint8_t> bytes(packed_str.begin(), packed_str.end());

  metadata_map metadata;
  metadata["type"] = std::to_string(type_id);
  metadata["size"] = std::to_string(bytes.size());

  memory_nome_->write_data_to_memory(bytes, metadata); // ������ � MMF
}


