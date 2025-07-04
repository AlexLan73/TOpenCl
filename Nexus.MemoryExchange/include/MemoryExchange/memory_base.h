#pragma once
// memory_base.h

#include "interfaces/i_memory_config_channel.h"
#include "shared_data_types.h"
#include <string>
#include <vector>
#include <windows.h>    // Для WinAPI
#include <thread>      // Для std::thread
#include <atomic>      // Для std::atomic

// Важно: Отменить макрос unpack из Windows.h, если он определен
#ifdef unpack
#undef unpack
#endif

class MemoryBase {
public:
  MemoryBase(const std::string& name_memory, type_block_memory type_block_memory_,
    size_t data_segment_size, const callback_data_meta_data& callBack);
  ~MemoryBase();

  MemoryBase(const MemoryBase&) = delete;
  MemoryBase& operator=(const MemoryBase&) = delete;

  void write_data(const std::vector<uint8_t>& data, const metadata_map& metadata);
  metadata_map get_command_control();
  void set_command_control(const metadata_map& metadata);
  void clear_command_control();

private:
  void event_loop();
  std::wstring to_wstring(const std::string& str); // Добавлен сюда, чтобы быть членом класса
  metadata_map parse_control_string(const char* str);
  std::string format_control_string(const metadata_map& metadata);

  HANDLE h_control_map_file_ = nullptr;
  HANDLE h_data_map_file_ = nullptr;
  HANDLE h_event_ = nullptr;

  std::string name_memory_data_;
  std::string name_memory_data_control_;

  callback_data_meta_data call_back_;
  std::thread event_thread_;
  std::atomic<bool> running_{ false };

  const size_t control_size_ = 8 * 1024;
  size_t data_segment_size_; // Исправлен тип на size_t
};

