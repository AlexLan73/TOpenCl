// ReSharper disable CppClangTidyClangDiagnosticInvalidUtf8
#include "pch.h"
#include <algorithm>
#include "MemoryExchange/BasicMemoryMd.h"

#pragma region  ==- Constructor -==
BasicMemoryMd::BasicMemoryMd(const std::string& eventName, int size, const std::string& controlName, Callback callBack,
	HANDLE sendTo) : size_(size),
  send_to_(sendTo),
  call_back_(callBack),
  stop_requested_(false)
{
  // Создаем или открываем файл отображения памяти
  h_file_mapping_ = CreateFileMappingA(
    INVALID_HANDLE_VALUE, // Виртуальный файл под память
    nullptr,
    PAGE_READWRITE,
    0,
    size,
    controlName.c_str());

  if (!h_file_mapping_) {
    throw std::runtime_error("Failed to create or open file mapping");
  }

  // Отображаем память в адресное пространство
  p_data_ = MapViewOfFile(h_file_mapping_, FILE_MAP_ALL_ACCESS, 0, 0, size);
  if (!p_data_) {
    CloseHandle(h_file_mapping_);
    throw std::runtime_error("Failed to map view of file");
  }

  // Создаем или открываем событие
  h_event_ = CreateEventA(nullptr, FALSE, FALSE, eventName.c_str());  // auto-reset
  if (!h_event_) {
    UnmapViewOfFile(p_data_);
    CloseHandle(h_file_mapping_);
    throw std::runtime_error("Failed to create or open event");
  }

  // Запускаем поток ожидания события
  worker_thread_ = std::thread(&BasicMemoryMd::wait_event_loop, this);

}
#pragma endregion 

#pragma region ===-- Distructor --===
BasicMemoryMd::~BasicMemoryMd()
{
  Dispose();
}

void BasicMemoryMd::Dispose()
{
  stop_requested_ = true;
  if (stop_requested_.exchange(true)) {
    return; // Уже остановлен
  }
  if (worker_thread_.joinable()) {
    SetEvent(h_event_);  // чтобы выйти из ожидания
    worker_thread_.join();
  }

  if (h_event_) {
    CloseHandle(h_event_);
    h_event_ = nullptr;
  }
  if (p_data_) {
    UnmapViewOfFile(p_data_);
    p_data_ = nullptr;
  }
  if (h_file_mapping_) {
    CloseHandle(h_file_mapping_);
    h_file_mapping_ = nullptr;
  }
  // НЕ закрываем _sendTo — он приходит из вне, ответственность за него снаружи

}
#pragma endregion

void BasicMemoryMd::WriteMetaMap(const metadata_map& map)
{
  if (map.empty()) return;
  std::string s = convert_dict_to_string(map);
  WriteMeta(s);
}

void BasicMemoryMd::WriteMeta(const std::string& text){
  if (text.empty()) return;

  std::lock_guard<std::mutex> lock(sync_lock_);
  size_t len = min(text.size(), static_cast<size_t>(size_));
  memcpy(p_data_, text.data(), len);

  // Заполняем остаток нулями, если есть место
  if (len < static_cast<size_t>(size_)) {
    memset(static_cast<char*>(p_data_) + len, 0, size_ - len);
  }

  // Сигналим другому процессу
  if (send_to_) {
    SetEvent(send_to_);
  }
}


void BasicMemoryMd::wait_event_loop()
{
  while (!stop_requested_.load()) {
    DWORD waitResult = WaitForSingleObject(h_event_, 1000); // timeout 1000ms
    if (stop_requested_.load()) break;
    if (waitResult == WAIT_OBJECT_0) {
      metadata_map map;
      {
        std::lock_guard<std::mutex> lock(sync_lock_);
        // Читаем из общей памяти
        std::string buffer(static_cast<char*>(p_data_), size_);
        // Обрезаем нули (как TrimEnd('\0'))
        buffer.erase(std::find(buffer.begin(), buffer.end(), '\0'), buffer.end());

        if (!buffer.empty()) {
          map = convert_string_to_dict(buffer);

          // Очищаем память
          memset(p_data_, 0, size_);
        }
      }

      if (!map.empty()) {
        try {
          call_back_(map);
        }
        catch (...) {
          // При ошибке вызываем колбэк с пустым словарём (как _callBack(null) в C#)
          call_back_(metadata_map{});
        }
      }
    }
    else if (waitResult == WAIT_TIMEOUT) {
      // таймаут — просто продолжаем цикл
    }
    else {
      // Ошибка ожидания - выйдем
      break;
    }
  }

}

std::string BasicMemoryMd::convert_dict_to_string(const metadata_map& dic){
  std::ostringstream oss;
  for (auto it = dic.begin(); it != dic.end(); ++it) {
    oss << it->first << "=" << it->second << ";";
  }
  return oss.str();
}

metadata_map BasicMemoryMd::convert_string_to_dict(const std::string& str){
  metadata_map map;
  std::istringstream iss(str);
  std::string token;

  while (std::getline(iss, token, ';')) {
    if (token.empty()) continue;
    size_t pos = token.find('=');
    if (pos == std::string::npos) continue;
    std::string key = token.substr(0, pos);
    std::string value = token.substr(pos + 1);
    map[key] = value;
  }
  return map;
}
