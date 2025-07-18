// memory_base.cpp

#include "pch.h"
#include "include/MemoryExchange/memory_base.h"

#include <iostream>

#include "interfaces/ModuleNameProvider.h"


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
  name_module_ = ModuleNameProvider::instance().get_name();

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

void MemoryBase::event_loop() {
  constexpr char handshake_prefix[] = "server";
 // const std::string myModule = name_module_; // ModuleNameProvider::instance().get_name();
  const std::string myHandshakeKey = std::string(handshake_prefix) + name_module_; // "serverCUDA" и др.

  // При необходимости вынесите commands в отдельный enum/таблицу.
  enum class MetaCommand { HandshakeOk, WorkOk, NotHandled };

  auto analyze_and_respond = [&](metadata_map& metadata) -> MetaCommand {
    // 1. Handshake: "state" == "serverCUDA"
    auto itState = metadata.find("state");
    if (itState != metadata.end() && itState->second == myHandshakeKey) {
      // Ответить "ok" в MD
      metadata["command"] = "ok";
      set_command_control(metadata);
      return MetaCommand::HandshakeOk;
    }

    // 2. Work command: "work" == ""
    const auto it_work = metadata.find("work");
    if (it_work != metadata.end() && it_work->second.empty()) {
      metadata["work"] = "ok";
      set_command_control(metadata);
      return MetaCommand::WorkOk;
    }

    // 3. Любая другая команда
    return MetaCommand::NotHandled;
    };

  while (running_.load()) {
    if (WaitForSingleObject(h_event_, 1000) == WAIT_OBJECT_0) {
      if (!running_.load()) break;

      metadata_map metadata = get_command_control();

      if (metadata.empty()) {
        if (call_back_) call_back_({ {}, metadata });
        continue;
      }

      // Автоматический анализ и ответ на handshake/workd
      if (analyze_and_respond(metadata) != MetaCommand::NotHandled) {
        // Перехватили и отработали — не передаём наверх
        continue;
      }

      // Стандартное чтение данных, если есть
      size_t dataSize = 0;
      auto it = metadata.find("size");
      if (it != metadata.end()) dataSize = std::stoul(it->second);

      std::vector<uint8_t> data;
      if (dataSize > 0) {
        data.resize(dataSize);
        LPVOID pDataBuf = MapViewOfFile(h_data_map_file_, FILE_MAP_READ, 0, 0, dataSize);
        if (pDataBuf != nullptr) {
          memcpy(data.data(), pDataBuf, dataSize);
          UnmapViewOfFile(pDataBuf);
        }
        else {
          data.clear();
          std::cerr << "Ошибка: не удалось получить MapViewOfFile для данных размером " << dataSize << std::endl;
        }
      }

      // Любая другая ситуация — колбэк в приложение
      if (call_back_) call_back_({ data, metadata });
    }
  }
}

/*
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
*/

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

/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 Вот как можно переписать метод event_loop для MemoryBase с обработкой команд и автоматическим ответом, согласно вашему сценарию обмена и handshake-протоколу.

### Переписанный event_loop с автоматической обработкой handshake-команд

В этом примере:

- Ключевые слова и идентификаторы команд вынесены в enum или constexpr для расширяемости.
- Имя зарезервированного модуля (например, "CUDA") берётся из синглтона (ModuleNameProvider::instance().get_name()).
- Автоматически анализируются управляющие команды ("state", "workd" и т.д.).
- Ответы формируются прямо в метаданных и отправляются обратно в память.
- Все остальные случаи передаются в call_back_, как и просили.

```cpp
void MemoryBase::event_loop() {
    constexpr char handshake_prefix[] = "server";
    const std::string myModule = ModuleNameProvider::instance().get_name();
    const std::string myHandshakeKey = std::string(handshake_prefix) + myModule; // "serverCUDA" и др.

    // При необходимости вынесите commands в отдельный enum/таблицу.
    enum class MetaCommand { HandshakeOk, WorkOk, NotHandled };

    auto analyze_and_respond = [&](metadata_map& metadata) -> MetaCommand {
        // 1. Handshake: "state" == "serverCUDA"
        auto itState = metadata.find("state");
        if (itState != metadata.end() && itState->second == myHandshakeKey) {
            // Ответить "ok" в MD
            metadata["command"] = "ok";
            set_command_control(metadata);
            return MetaCommand::HandshakeOk;
        }

        // 2. Work command: "workd" == ""
        auto itWorkd = metadata.find("workd");
        if (itWorkd != metadata.end() && itWorkd->second.empty()) {
            metadata["workd"] = "ok";
            set_command_control(metadata);
            return MetaCommand::WorkOk;
        }

        // 3. Любая другая команда
        return MetaCommand::NotHandled;
    };

    while (running_.load()) {
        if (WaitForSingleObject(h_event_, 1000) == WAIT_OBJECT_0) {
            if (!running_.load()) break;

            metadata_map metadata = get_command_control();

            if (metadata.empty()) {
                if (call_back_) call_back_({ {}, metadata });
                continue;
            }

            // Автоматический анализ и ответ на handshake/workd
            if (analyze_and_respond(metadata) != MetaCommand::NotHandled) {
                // Перехватили и отработали — не передаём наверх
                continue;
            }

            // Стандартное чтение данных, если есть
            size_t dataSize = 0;
            auto it = metadata.find("size");
            if (it != metadata.end()) dataSize = std::stoul(it->second);

            std::vector data;
            if (dataSize > 0) {
                data.resize(dataSize);
                LPVOID pDataBuf = MapViewOfFile(h_data_map_file_, FILE_MAP_READ, 0, 0, dataSize);
                if (pDataBuf != nullptr) {
                    memcpy(data.data(), pDataBuf, dataSize);
                    UnmapViewOfFile(pDataBuf);
                } else {
                    data.clear();
                    std::cerr << "Ошибка: не удалось получить MapViewOfFile для данных размером " << dataSize << std::endl;
                }
            }

            // Любая другая ситуация — колбэк в приложение
            if (call_back_) call_back_({ data, metadata });
        }
    }
}
```

### Особенности

- Ключевые handshake-команды ("state", "workd") легко расширять через enum или map.
- Ответы отправляются обратно только если команда известна и поддерживается.
- Вся остальная логика (обработка данных, нештатных команд) — через основной call_back_.

**Если появятся дополнительные команды (например, "reset", "pause" и т.д.), просто добавьте их в analyze_and_respond. Такой подход делает логику максимально расширяемой и централизованной.**

Если нужны заготовки кода для enum команд или шаблон для расширения, сообщите — подготовлю!

[1] https://pplx-res.cloudinary.com/image/private/user_uploads/73552077/5a98c447-6a4a-4a1e-b00c-92f0c8f8de15/Costoianii-obmena.jpg
[2] https://pplx-res.cloudinary.com/image/private/user_uploads/73552077/315891f0-9053-403e-8c50-6c6153e27ba2/Obmena-soobshcheniiami-klient-server.jpg
[3] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/e44cf27d-9535-4717-9207-50f932d9d405/Opisanie-protokola-01.txt
[4] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/26c8d8bb-d372-46e5-884c-c485072a83a2/Protocol.h
[5] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/f677a794-f2f4-4bc8-8a2a-1b6da605341e/DataContext.h
[6] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/45d4849d-f8f1-4688-80c8-fb1be7014c21/IDataContext.h
[7] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/fcfed0c9-cb02-4ffc-a0d8-5ce2a24e57cc/IProtocol.h
[8] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/718fcce3-1e05-4a14-8798-e7a5f3dca437/kit_enums.h
[9] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/e04294e6-1503-446a-b2dd-ddae8b97ea43/DataContext.cpp
[10] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/6fbc5171-d17d-4c48-810e-20793ef1a133/Protocol.cpp
[11] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/180958a1-30c2-4739-bddd-ab078258e884/i_memory_data_handler.h
[12] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/bfc9feb9-b55a-4766-85dc-c95a381719e5/memory_base.h
[13] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/10d9e6d5-f9eb-410b-96ae-b7bc99be3c07/memory_nome.h
[14] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/b1555287-8161-4d44-b6fe-d88c9b094ac1/memory_processor.h
[15] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/ce2d1963-b597-409b-8db7-9c489b1fc627/my_msgpack_adapters.h
[16] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/d21af581-06a5-4a67-b7c3-736ff11f9142/shared_data_types.h
[17] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/913d27c9-a2c8-45e9-8127-f24197475218/memory_base.cpp
[18] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/4ecee3c6-7f68-4e24-89aa-116118df1bd6/memory_nome.cpp
[19] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/a7cb0425-99f4-496e-a059-0bceac3348a9/memory_processor.cpp
[20] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/ce7c5716-7bc4-4596-a7f5-b52ceb173e60/RecDataMetaData.cs
[21] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/cf13874c-74e4-45f1-95e7-cdd987d85e81/MemoryBase.cs
[22] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/dbdfe94c-c21c-475c-9158-5e14fbd08ebe/MemoryNome.cs
[23] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/83eeed7b-21ae-4964-bb3d-e5abc69e47a3/ModuleCore.cs
[24] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/73552077/0daf6087-9c95-4fce-8ae6-3a241c637b85/ServerHandshake.cs
 
 */