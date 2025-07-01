// memory_base.cpp

#include "pch.h"
#include "include/MemoryExchange/memory_base.h"

#include <iostream>


// ��������������� ������� ��� ����������� std::string (UTF-8) � std::wstring (UTF-16) ��� WinAPI
// ������ ��� ����� ������
std::wstring MemoryBase::to_wstring(const std::string& str) {
  if (str.empty()) return std::wstring();
  const int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), int(str.size()), nullptr, 0);
  std::wstring wstr(size_needed, 0);
  MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), wstr.data(), size_needed);
  return wstr;
}

// --- ����������� ---
MemoryBase::MemoryBase(const std::string& name_memory, const type_block_memory  type_block_memory_, const size_t data_segment_size,
                       const callback_data_meta_data& callBack)
		: data_segment_size_(data_segment_size),  call_back_(callBack) {
	const std::string name_memory_control = name_memory + "Control";
	const std::wstring w_name_control = to_wstring(name_memory_control);
	const std::wstring w_name_data = to_wstring(name_memory);
	const std::wstring w_event_name = L"Global\\Event" + to_wstring(name_memory); // ���������� to_w_string ��� �����

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

// --- ���������� ---
MemoryBase::~MemoryBase() {
  if (running_.load()) {
    running_ = false;
    if (h_event_) SetEvent(h_event_); // ����������, ��� ����� �������
    if (event_thread_.joinable()) {
      event_thread_.join();
    }
  }
  if (h_event_) CloseHandle(h_event_);
  if (h_control_map_file_) CloseHandle(h_control_map_file_);
  if (h_data_map_file_) CloseHandle(h_data_map_file_);
}

// --- ��������� ������ ---
void MemoryBase::write_data(const std::vector<uint8_t>& data, const metadata_map& metadata) {
  if (data.size() > data_segment_size_) {
    throw std::runtime_error("������ ������ ��������� ������ ����������� �������� ������.");
  }

  const LPVOID pDataBuf = MapViewOfFile(h_data_map_file_, FILE_MAP_ALL_ACCESS, 0, 0, data_segment_size_);
  if (pDataBuf == nullptr) {
    throw std::runtime_error("�� ������� �������� View Of File ��� ������ ������.");
  }

  ZeroMemory(pDataBuf, data_segment_size_); // ������� ������
  CopyMemory(pDataBuf, data.data(), data.size()); // ����������� ������
  UnmapViewOfFile(pDataBuf);

  set_command_control(metadata); // ������ ���������� � ������ �������
}

metadata_map MemoryBase::get_command_control() {
	const LPVOID pBuf = MapViewOfFile(h_control_map_file_, FILE_MAP_READ, 0, 0, control_size_);
  if (pBuf == nullptr) return {};
	const std::string controlStr(static_cast<char*>(pBuf)); // ��������� ���� ����������� ����� �� ����-�����������
  UnmapViewOfFile(pBuf);
  return parse_control_string(controlStr.c_str());
}

void MemoryBase::set_command_control(const metadata_map& metadata) {
	const std::string controlStr = format_control_string(metadata);
  // ���������, ����� ������ ���������� �� ��������� ������ ������������ ������
  if (controlStr.length() + 1 > control_size_) { // +1 ��� ����-�����������
    throw std::runtime_error("���������� ��������� ������ ������������ ������.");
  }

	const LPVOID pBuf = MapViewOfFile(h_control_map_file_, FILE_MAP_ALL_ACCESS, 0, 0, control_size_);
  if (pBuf == nullptr) {
    throw std::runtime_error("�� ������� �������� View Of File ��� ������ ����������.");
  }

  ZeroMemory(pBuf, control_size_); // ������� ���� �����
  CopyMemory(pBuf, controlStr.c_str(), controlStr.length()); // �������� ������
  // �� ����� ���� ������� ����-����������, ZeroMemory ��� ��� �������� �� ��������� controlStr.length()
  UnmapViewOfFile(pBuf);
  ::SetEvent(h_event_); // ������������� � ����� ������
}

// �������� �� ������� memory_base.cpp
void MemoryBase::clear_command_control() {
  std::cout << "[MemoryBase] ������� ����������� ������..." << std::endl;
  const LPVOID pBuf = MapViewOfFile(h_control_map_file_, FILE_MAP_ALL_ACCESS, 0, 0, control_size_);
  if (pBuf == nullptr) {
    throw std::runtime_error("�� ������� �������� View Of File ��� �������.");
  }
  ZeroMemory(pBuf, control_size_);
  UnmapViewOfFile(pBuf);
  std::cout << "[MemoryBase] ������ �������. ����� ������." << std::endl;
  ::SetEvent(h_event_);
}

// --- ��������� ������ ---
void MemoryBase::event_loop() {
  while (running_.load()) {
    // ������� ������� � ��������� � 1 �������
    if (WaitForSingleObject(h_event_, 1000) == WAIT_OBJECT_0) {
      // ������� ��������, ���� ��� ������� ������ � ���������� ������
      if (!running_.load()) break;

      metadata_map metadata = get_command_control();

      // �������� 1: ������� ������, �� ���������� ��� (��� ��� ������).
      // ��� ����� ���� ������ ������ ����������� ��� �������.
      // �� ��� ����� ������ ��������� ����������, ������� ������ ������.
      if (metadata.empty()) {
        if (call_back_) {
          call_back_({ {}, metadata });
        }
        continue; // ��������� � ��������� �������� �����
      }

      // �������� 2: ���������� ����, ����� ��������� �������� ���� ������.
      auto it = metadata.find("size");
      size_t dataSize = (it != metadata.end()) ? std::stoul(it->second) : 0;

      std::vector<uint8_t> data;
      if (dataSize > 0) {
        // �������� ������ � ������� ����� ������� ��� �������������
        data.resize(dataSize);

        LPVOID pDataBuf = MapViewOfFile(h_data_map_file_, FILE_MAP_READ, 0, 0, dataSize);
        if (pDataBuf != nullptr) {
          // �������� ������ �������� � ������ �������
          memcpy(data.data(), pDataBuf, dataSize);
          UnmapViewOfFile(pDataBuf);
        }
        else {
          // ���� �� ������� ���������, ������� ������ �� ������ ������
          data.clear();
          std::cerr << "������: �� ������� �������� MapViewOfFile ��� ������ �������� " << dataSize << std::endl;
        }
      }

      // �������� ������ � ����������� ������� (���� ���� ��� ������ ��-�� ������)
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
  for (const auto& pair : metadata) { // ����������: ����������� ranged-based for loop
    ss << pair.first << "=" << pair.second << ";";
  }
  return ss.str();
}

