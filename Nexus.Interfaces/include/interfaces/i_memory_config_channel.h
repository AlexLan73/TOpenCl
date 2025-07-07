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

// --- Идентификаторы типов данных ---
enum data_type_ids : uint32_t {
  e_logger = 0,
  e_date_time_variable = 1,
  e_vector = 2,
  e_matrix = 3,
  e_rec_result = 4,
  e_value = 5
};
#define COUNT_TYPE_DATA_CHANNEL 6

// e_logger = 0,
struct ILoggerChannel  {
  uint32_t id;
  std::string module;
  std::string log;
  logger_send_enum_memory code;
};

//e_date_time_variable = 1,
struct IDateTimeVariableChannel {
  uint32_t id;
  std::string date_time;
  float variable;
};

// e_vector = 2,
struct IVectorChannel {
  uint32_t id;
  std::vector<double> values;
};

// e_vector = 3,
struct IMatrixChannel {
  uint32_t id;
  uint32_t i;
  uint32_t j;
  std::vector<double> values; // Все элементы матрицы по порядку // NOLINT(clang-diagnostic-invalid-utf8)
};

// e_rec_result = 4,
struct IRecResultChannel {
  uint32_t id;
  uint32_t n_fft;
  uint32_t m_channel;
  double time_fft;
  double time_load_data;
  double time_waite_data;
};

// e_value = 5
struct IValueChannel {
  uint32_t id;
  double value;
};

// e_dt_record = 6,


struct IChannelTypeData
{
  virtual void send_channel(std::vector<ILoggerChannel>& data) {}
  virtual void send_channel(std::vector<IDateTimeVariableChannel>& data) {}
  virtual void send_channel(std::vector<IVectorChannel>& data) {}
  virtual void send_channel(std::vector<IMatrixChannel>& data) {}
  virtual void send_channel(std::vector<IRecResultChannel>& data) {}
  virtual void send_channel(std::vector<IValueChannel>& data) {}
};
