//  memory_nome.h
#pragma once

// Подключаем, чтобы видеть все типы данных  // NOLINT(clang-diagnostic-invalid-utf8)
#include "interfaces/i_memory_config_channel.h"
#include "memory_base.h"


#include <string>
#include <memory>       // Для std::unique_ptr


class MemoryNome{
public:
  MemoryNome(const std::string& name_memory, server_client role, callback_data_meta_data callback);
  ~MemoryNome() = default;

  MemoryNome(const MemoryNome&) = delete;
  MemoryNome& operator=(const MemoryNome&) = delete;

  void write_data_to_memory(const std::vector<uint8_t>& bytes, const metadata_map& map);

	void initialize_handshake(bool is_client, std::string identifier="");

  // Проверяет метаданные на канале, в который мы ПИШЕМ.
  // Позволяет узнать, прочитал ли получатель наши данные.
  metadata_map check_write_channel_control();

  // Очищает метаданные на канале, из которого мы ЧИТАЕМ.
  // Это нужно делать после того, как мы обработали входящие данные.
  void clear_read_channel_control();
private:
  std::unique_ptr<MemoryBase> memory_read_; // Используем unique_ptr
  std::unique_ptr<MemoryBase> memory_write_; // Используем unique_ptr
  std::string client_name_ram_ = "client";
  std::string name_module_;

};



