// memory_base.cpp

#include "pch.h"
#include "include/MemoryExchange/memory_base.h"

#include <iostream>


// Вспомогательная функция для конвертации std::string (UTF-8) в std::wstring (UTF-16) для WinAPI
// Теперь это метод класса
std::wstring MemoryBase::to_wstring(const std::string& str) {
  if (str.empty()) return std::wstring();
  const int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), int(str.size()), nullptr, 0);
  std::wstring wstr(size_needed, 0);
  MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), wstr.data(), size_needed);
  return wstr;
}

// --- КОНСТРУКТОР ---
MemoryBase::MemoryBase(const std::string& name_memory, const type_block_memory  type_block_memory_, const size_t data_segment_size,
                       const callback_data_meta_data& callBack)
		: data_segment_size_(data_segment_size),  call_back_(callBack) {
	const std::string name_memory_control = name_memory + "Control";
	const std::wstring w_name_control = to_wstring(name_memory_control);
	const std::wstring w_name_data = to_wstring(name_memory);
	const std::wstring w_event_name = L"Global\\Event" + to_wstring(name_memory); // Используем to_w_string как метод

  h_control_map_file_ = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, static_cast<DWORD>(control_size_), w_name_control.c_str());
  if (h_control_map_file_ == nullptr) throw std::runtime_error("Failed to create control file mapping.");

  h_data_map_file_ = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, static_cast<DWORD>(data_segment_size), w_name_data.c_str());
  if (h_data_map_file_ == nullptr) {
    CloseHandle(h_control_map_file_);
    throw std::runtime_error("Failed to create data file mapping.");
  }

  h_event_ = CreateEventW(nullptr, FALSE, FALSE, w_event_name.c_str());
  if (h_event_ == nullptr) {
    CloseHandle(h_control_map_file_);
    CloseHandle(h_data_map_file_);
    throw std::runtime_error("Failed to create event.");
  }

  if (type_block_memory_ == type_block_memory::read && call_back_) {
    running_ = true;
    event_thread_ = std::thread(&MemoryBase::event_loop, this);
  }
}

// --- ДЕСТРУКТОР ---
MemoryBase::~MemoryBase() {
  if (running_.load()) {
    running_ = false;
    if (h_event_) SetEvent(h_event_); // Убеждаемся, что хэндл валиден
    if (event_thread_.joinable()) {
      event_thread_.join();
    }
  }
  if (h_event_) CloseHandle(h_event_);
  if (h_control_map_file_) CloseHandle(h_control_map_file_);
  if (h_data_map_file_) CloseHandle(h_data_map_file_);
}

// --- ПУБЛИЧНЫЕ МЕТОДЫ ---
void MemoryBase::write_data(const std::vector<uint8_t>& data, const metadata_map& metadata) {
  if (data.size() > data_segment_size_) {
    throw std::runtime_error("Размер данных превышает размер выделенного сегмента памяти.");
  }

  const LPVOID pDataBuf = MapViewOfFile(h_data_map_file_, FILE_MAP_ALL_ACCESS, 0, 0, data_segment_size_);
  if (pDataBuf == nullptr) {
    throw std::runtime_error("Не удалось получить View Of File для записи данных.");
  }

  ZeroMemory(pDataBuf, data_segment_size_); // Очистка памяти
  CopyMemory(pDataBuf, data.data(), data.size()); // Копирование данных
  UnmapViewOfFile(pDataBuf);

  set_command_control(metadata); // Запись метаданных и подача сигнала
}

metadata_map MemoryBase::get_command_control() {
	const LPVOID pBuf = MapViewOfFile(h_control_map_file_, FILE_MAP_READ, 0, 0, control_size_);
  if (pBuf == nullptr) return {};
	const std::string controlStr(static_cast<char*>(pBuf)); // Считываем весь контрольный буфер до нуль-терминатора
  UnmapViewOfFile(pBuf);
  return parse_control_string(controlStr.c_str());
}

void MemoryBase::set_command_control(const metadata_map& metadata) {
	const std::string controlStr = format_control_string(metadata);
  // Проверяем, чтобы строка метаданных не превышала размер контрольного буфера
  if (controlStr.length() + 1 > control_size_) { // +1 для нуль-терминатора
    throw std::runtime_error("Метаданные превышают размер контрольного буфера.");
  }

	const LPVOID pBuf = MapViewOfFile(h_control_map_file_, FILE_MAP_ALL_ACCESS, 0, 0, control_size_);
  if (pBuf == nullptr) {
    throw std::runtime_error("Не удалось получить View Of File для записи метаданных.");
  }

  ZeroMemory(pBuf, control_size_); // Очищаем весь буфер
  CopyMemory(pBuf, controlStr.c_str(), controlStr.length()); // Копируем данные
  // Не нужно явно ставить нуль-терминатор, ZeroMemory уже все обнулила за пределами controlStr.length()
  UnmapViewOfFile(pBuf);
  ::SetEvent(h_event_); // Сигнализируем о новых данных
}

// Добавлен из старого memory_base.cpp
void MemoryBase::clear_command_control() {
  std::cout << "[MemoryBase] Очистка контрольной памяти..." << std::endl;
  const LPVOID pBuf = MapViewOfFile(h_control_map_file_, FILE_MAP_ALL_ACCESS, 0, 0, control_size_);
  if (pBuf == nullptr) {
    throw std::runtime_error("Не удалось получить View Of File для очистки.");
  }
  ZeroMemory(pBuf, control_size_);
  UnmapViewOfFile(pBuf);
  std::cout << "[MemoryBase] Память очищена. Подаю сигнал." << std::endl;
  ::SetEvent(h_event_);
}

// --- ПРИВАТНЫЕ МЕТОДЫ ---
void MemoryBase::event_loop() {
  while (running_.load()) {
    // Ожидаем событие с таймаутом в 1 секунду
    if (WaitForSingleObject(h_event_, 1000) == WAIT_OBJECT_0) {
      // Двойная проверка, если был получен сигнал о завершении работы
      if (!running_.load()) break;

      metadata_map metadata = get_command_control();

      // Ситуация 1: Событие пришло, но метаданных нет (или они пустые).
      // Это может быть просто сигнал пробуждения или очистки.
      // Мы все равно должны уведомить подписчика, передав пустые данные.
      if (metadata.empty()) {
        if (call_back_) {
          call_back_({ {}, metadata });
        }
        continue; // Переходим к следующей итерации цикла
      }

      // Ситуация 2: Метаданные есть, нужно прочитать основной блок данных.
      auto it = metadata.find("size");
      size_t dataSize = (it != metadata.end()) ? std::stoul(it->second) : 0;

      std::vector<uint8_t> data;
      if (dataSize > 0) {
        // Выделяем память в векторе ПЕРЕД чтением для эффективности
        data.resize(dataSize);

        LPVOID pDataBuf = MapViewOfFile(h_data_map_file_, FILE_MAP_READ, 0, 0, dataSize);
        if (pDataBuf != nullptr) {
          // Копируем данные напрямую в память вектора
          memcpy(data.data(), pDataBuf, dataSize);
          UnmapViewOfFile(pDataBuf);
        }
        else {
          // Если не удалось прочитать, очищаем вектор на всякий случай
          data.clear();
          std::cerr << "Ошибка: не удалось получить MapViewOfFile для данных размером " << dataSize << std::endl;
        }
      }

      // Вызываем колбэк с полученными данными (даже если они пустые из-за ошибки)
      if (call_back_) {
        call_back_({ data, metadata });
      }
    }
  }
}

metadata_map MemoryBase::parse_control_string(const char* control_str) {
  metadata_map metadata;
  if (control_str == nullptr) return metadata;
  const std::string str(control_str);
  std::stringstream ss(str);
  std::string segment;
  while (std::getline(ss, segment, ';')) {
    if (segment.empty()) continue;
    const std::string::size_type pos = segment.find('=');
    if (pos != std::string::npos) {
      metadata[segment.substr(0, pos)] = segment.substr(pos + 1);
    }
  }
  return metadata;
}

std::string MemoryBase::format_control_string(const metadata_map& metadata) {
  std::stringstream ss;
  for (const auto& pair : metadata) { // Исправлено: использован ranged-based for loop
    ss << pair.first << "=" << pair.second << ";";
  }
  return ss.str();
}

