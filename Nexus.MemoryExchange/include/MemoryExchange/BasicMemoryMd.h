#pragma once
#include <windows.h>
#include <string>
#include <unordered_map>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "interfaces/i_memory_config_channel.h"

//#include "interfaces/i_memory_config_channel.h"


class BasicMemoryMd
{
  using Callback = std::function<void(const metadata_map&)>;
public:
  BasicMemoryMd(const std::string& eventName,
    int size,
    const std::string& controlName,
    Callback callBack,
    HANDLE sendTo);

  ~BasicMemoryMd();
  void Dispose();

  void WriteMetaMap(const metadata_map& map); 
  void WriteMeta(const std::string& text);

private:
  void wait_event_loop();
  static std::string convert_dict_to_string(const metadata_map& dic);
  static metadata_map convert_string_to_dict(const std::string& str);

  int size_;
  HANDLE send_to_;

  HANDLE h_file_mapping_ = nullptr;
  LPVOID p_data_ = nullptr;
  HANDLE h_event_ = nullptr;

  Callback call_back_;
  std::thread worker_thread_;
  std::mutex sync_lock_;
  std::atomic<bool> stop_requested_;

};

/*
 
 
using MapCommands = std::unordered_map<std::string, std::string>;

class BasicMemoryMd {
public:

    BasicMemoryMd(const std::string& eventName,
                  int size,
                  const std::string& controlName,
                  Callback callBack,
                  HANDLE sendTo)
        : _size(size),
          _sendTo(sendTo),
          _callBack(callBack),
          _stopRequested(false)
    {
        // Создаем или открываем файл отображения памяти
        _hFileMapping = CreateFileMappingA(
            INVALID_HANDLE_VALUE, // Виртуальный файл под память
            nullptr,
            PAGE_READWRITE,
            0,
            size,
            controlName.c_str());

        if (!_hFileMapping) {
            throw std::runtime_error("Failed to create or open file mapping");
        }

        // Отображаем память в адресное пространство
        _pData = MapViewOfFile(_hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, size);
        if (!_pData) {
            CloseHandle(_hFileMapping);
            throw std::runtime_error("Failed to map view of file");
        }

        // Создаем или открываем событие
        _hEvent = CreateEventA(nullptr, FALSE, FALSE, eventName.c_str());  // auto-reset
        if (!_hEvent) {
            UnmapViewOfFile(_pData);
            CloseHandle(_hFileMapping);
            throw std::runtime_error("Failed to create or open event");
        }

        // Запускаем поток ожидания события
        _workerThread = std::thread(&BasicMemoryMd::WaitEventLoop, this);
    }

    ~BasicMemoryMd() {
        Dispose();
    }

    void Dispose() {
        if (_stopRequested.exchange(true)) {
            return; // Уже остановлен
        }
        if (_workerThread.joinable()) {
            SetEvent(_hEvent);  // чтобы выйти из ожидания
            _workerThread.join();
        }

        if (_hEvent) {
            CloseHandle(_hEvent);
            _hEvent = nullptr;
        }
        if (_pData) {
            UnmapViewOfFile(_pData);
            _pData = nullptr;
        }
        if (_hFileMapping) {
            CloseHandle(_hFileMapping);
            _hFileMapping = nullptr;
        }
        // НЕ закрываем _sendTo — он приходит из вне, ответственность за него снаружи
    }

    void WriteMetaMap(const MapCommands& map) {
        if (map.empty()) return;
        std::string s = ConvertDictToString(map);
        WriteMeta(s);
    }

    void WriteMeta(const std::string& text) {
        if (text.empty()) return;

        std::lock_guard<std::mutex> lock(_syncLock);
        size_t len = std::min(text.size(), static_cast<size_t>(_size));
        memcpy(_pData, text.data(), len);

        // Заполняем остаток нулями, если есть место
        if (len < static_cast<size_t>(_size)) {
            memset(static_cast<char*>(_pData) + len, 0, _size - len);
        }

        // Сигналим другому процессу
        if (_sendTo) {
            SetEvent(_sendTo);
        }
    }

private:
    void WaitEventLoop() {
        while (!_stopRequested.load()) {
            DWORD waitResult = WaitForSingleObject(_hEvent, 1000); // timeout 1000ms
            if (_stopRequested.load()) break;
            if (waitResult == WAIT_OBJECT_0) {
                MapCommands map;

                {
                    std::lock_guard<std::mutex> lock(_syncLock);
                    // Читаем из общей памяти
                    std::string buffer(static_cast<char*>(_pData), _size);
                    // Обрезаем нули (как TrimEnd('\0'))
                    buffer.erase(std::find(buffer.begin(), buffer.end(), '\0'), buffer.end());

                    if (!buffer.empty()) {
                        map = ConvertStringToDict(buffer);

                        // Очищаем память
                        memset(_pData, 0, _size);
                    }
                }

                if (!map.empty()) {
                    try {
                        _callBack(map);
                    }
                    catch (...) {
                        // При ошибке вызываем колбэк с пустым словарём (как _callBack(null) в C#)
                        _callBack(MapCommands{});
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

    static std::string ConvertDictToString(const MapCommands& dic) {
        std::ostringstream oss;
        for (auto it = dic.begin(); it != dic.end(); ++it) {
            oss << it->first << "=" << it->second << ";";
        }
        return oss.str();
    }

    static MapCommands ConvertStringToDict(const std::string& str) {
        MapCommands map;
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

};

 */