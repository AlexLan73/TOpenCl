#pragma once
// SharedDataTypes.h


#include <string>
#include <vector>
#include <cstdint>
#include <map>
#include <functional> // Для CallbackDataMetaData
#include "interfaces/i_memory_config_channel.h"
#include <msgpack.hpp>

  // --- Базовые типы для IPC ---

// --- Идентификаторы типов данных ---
enum data_type_ids : uint32_t {
  e_logger = 0,
  e_date_time_variable = 1,
  e_vector = 2,
  e_matrix = 3,
  e_rec_result = 4,
  e_dt_record = 5,
  e_value = 6
};

// --- Структуры данных ---
//enum class LoggerSendEnumMemory { Error = -1, Info = 0, Warning = 1 };
MSGPACK_ADD_ENUM(logger_send_enum_memory);

struct IdLogger :public ILogger {
  MSGPACK_DEFINE_ARRAY(id, module, log, code); // Исправлено: убран лишний символ
};

struct IdVector :public IVector {
  MSGPACK_DEFINE_ARRAY(id, values);
};

struct IdValue :public IValue {
  MSGPACK_DEFINE_ARRAY(id, value);
};

struct DateTimeVariable : public IDateTimeVariable{
  MSGPACK_DEFINE_ARRAY(id, date_time, variable);
};

struct IdMatrix :public IMatrix {
  MSGPACK_DEFINE_ARRAY(id, i, j, values); // Исправлено: Id был дважды
};

struct RecResult :public IRecResult{
  MSGPACK_DEFINE_ARRAY(id, n_fft, m_channel, time_fft, time_load_data, time_waite_data
  );
};

struct DtRecord {
  int32_t id;
  std::string date_time;
  std::vector<RecResult> dt_records;
  MSGPACK_DEFINE_ARRAY(id, date_time, dt_records);
};
