//   i_memory_config_channel.h
#pragma once
#include <functional>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <cstdint>

enum class type_block_memory { read, write };
enum class server_client { server, client };

//using metadata_map = std::map<std::string, std::string>;
using metadata_map = std::unordered_map<std::string, std::string>;

struct rec_data_meta_data {
  std::vector<uint8_t> bytes;
  metadata_map meta_data;
};

using callback_data_meta_data = std::function<void(const rec_data_meta_data&)>;

// --- Структуры данных ---  // NOLINT(clang-diagnostic-invalid-utf8)
enum class logger_send_enum_memory { error = -1, info = 0, warning = 1 };

struct SValueDt {
  double value;
  int64_t ticks;
};

// --- Идентификаторы типов данных ---
enum data_type_ids : uint32_t {
  e_logger = 0,
  e_id_value_dt_channel = 1,
  e_id_vec_value_dt_channel = 2,
};
#define COUNT_TYPE_DATA_CHANNEL 3

// e_logger = 0,
struct ILoggerChannel  {
  uint32_t id;
  std::string module;
  std::string log;
  logger_send_enum_memory code;
  int64_t ticks; // .NET ticks (100 нс с 01.01.0001)
};

// Канал одного измерения
// e_id_value_dt_channel = 1,
struct IIdValueDtChannel {
  uint32_t id;
  SValueDt value_dt;
};

// Канал вектора измерений
// e_id_vec_value_dt_channel = 2,
struct IIdVecValueDtChannel {
  uint32_t id;
  std::vector<SValueDt> vec_value_dt;
};

