#pragma once
#include <functional>
#include <map>
#include <string>
#include <vector>

enum class type_block_memory { read, write };
enum class server_client { server, client };

using metadata_map = std::map<std::string, std::string>;

struct rec_data_meta_data {
  std::vector<uint8_t> bytes;
  metadata_map meta_data;
};

using callback_data_meta_data = std::function<void(const rec_data_meta_data&)>;

// --- Структуры данных ---  // NOLINT(clang-diagnostic-invalid-utf8)
enum class logger_send_enum_memory { error = -1, info = 0, warning = 1 };

struct ILoggerChannel {
  uint32_t id;
  std::string module;
  std::string log;
  logger_send_enum_memory code;
};

struct IVectorChannel {
  uint32_t id;
  std::vector<double> values;
};

struct IValueChannel {
  uint32_t id;
  double value;
};


struct IDateTimeVariableChannel {
  uint32_t id;
  std::string date_time;
  float variable;
};

struct IMatrixChannel {
  uint32_t id;
  uint32_t i;
  uint32_t j;
  std::vector<double> values; // Все элементы матрицы по порядку // NOLINT(clang-diagnostic-invalid-utf8)
};

struct IRecResultChannel {
  uint32_t id;
  uint32_t n_fft;
  uint32_t m_channel;
  double time_fft;
  double time_load_data;
  double time_waite_data;
};

