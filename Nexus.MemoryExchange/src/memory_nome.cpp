//  memory_nome.cpp

// ReSharper disable CppMemberFunctionMayBeConst
#include "pch.h"

#include "MemoryExchange/memory_nome.h"

#include "MemoryExchange/memory_base.h"   // Должен быть подключен для определения MemoryBase
#include <iostream>                       // Для std::cout, std::cerr

constexpr size_t default_data_size = 1024 * 64; // 64 KB  // NOLINT(bugprone-implicit-widening-of-multiplication-result)

MemoryNome::MemoryNome(const std::string& name_memory, server_client role, callback_data_meta_data callback) {
  std::string readChannelName;
  std::string writeChannelName;

  if (role == server_client::server) {
    readChannelName = name_memory + "Read";
    writeChannelName = name_memory + "Write";
    std::cout << "[MemoryNome Server] Reading from: " << readChannelName << ", Writing to: " << writeChannelName << '\n';
  }
  else { // Client
    readChannelName = name_memory + "Write";
    writeChannelName = name_memory + "Read";
    std::cout << "[MemoryNome Client] Reading from: " << readChannelName << ", Writing to: " << writeChannelName << '\n';
  }

  // Использование make_unique
  memory_read_ = std::make_unique<MemoryBase>(readChannelName, type_block_memory::read, default_data_size, callback);
  memory_write_ = std::make_unique<MemoryBase>(writeChannelName, type_block_memory::write, default_data_size, nullptr);
}

void MemoryNome::write_data_to_memory(const std::vector<uint8_t>& bytes, const metadata_map& map) {
  try {
    memory_write_->write_data(bytes, map);
  }
  catch (const std::exception& e) {
    std::cerr << "Error writing data in MemoryNome: " << e.what() << std::endl;
  }
}
metadata_map MemoryNome::check_write_channel_control() {
  if (!memory_write_) return {};
  // Просто вызываем GetCommandControl у нашего канала записи
  return memory_write_->get_command_control();
}

void MemoryNome::clear_read_channel_control() {
  if (!memory_read_) return;
  // Вызываем ClearCommandControl у нашего канала чтения
  memory_read_->clear_command_control();
}