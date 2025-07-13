#pragma once
// SharedDataTypes.h

#include <string>
#include <vector>
#include <cstdint>
#include "interfaces/i_memory_config_channel.h"
#include <msgpack.hpp>
#include <sstream>
#include "CRC.h"
#include "my_msgpack_adapters.h"
  
// --- Структуры данных ---
//enum class LoggerSendEnumMemory { Error = -1, Info = 0, Warning = 1 };
//MSGPACK_ADD_ENUM(logger_send_enum_memory);

struct IdLogger :public ILoggerChannel {
  MSGPACK_DEFINE_ARRAY(id, module, log, code, ticks) // Исправлено: убран лишний символ
};

struct ISValueDt :public SValueDt {
  double value;
  int64_t ticks;
  MSGPACK_DEFINE(value, ticks)
};

struct IIdValueDt {
  uint32_t id;
  ISValueDt value_dt;
  MSGPACK_DEFINE(id, value_dt);
};

struct IdVecValue {
  uint32_t id;
  std::vector<ISValueDt> vec_value_dt;
  MSGPACK_DEFINE(id, vec_value_dt)
};

namespace my_msgpack
{
  template<typename T>
  inline std::vector<uint8_t> serialize_old(const T& val) {
    std::ostringstream buffer;
    msgpack::pack(buffer, val);
    std::string str = buffer.str();
    return std::vector<uint8_t>(str.begin(), str.end());
  }

  template<typename T>
  inline std::tuple<std::vector<uint8_t>, std::string> serialize_with_crc(const T& val) {
    // Сериализация через MessagePack
    std::ostringstream buffer;
    msgpack::pack(buffer, val);
    std::string str = buffer.str();
    std::vector<uint8_t> data(str.begin(), str.end());

    // Подсчёт CRC32 (можно выбрать другой алгоритм)
    uint32_t crc = CRC::Calculate(data.data(), data.size(), CRC::CRC_32());

    // Преобразование CRC в hex-строку
    std::ostringstream crc_ss;
    crc_ss << std::hex << std::uppercase << crc;
    std::string crc_str = crc_ss.str();

    // Возврат кортежа (данные, строка CRC)
    return std::make_tuple(data, crc_str);
  }
}


/*



4. Десериализация (приём данных)
// Для одного объекта
std::string packed = buffer.str();
msgpack::object_handle oh = msgpack::unpack(packed.data(), packed.size());
ILoggerChannel log2;
oh.get().convert(log2);

// Для вектора
msgpack::object_handle oh_vec = msgpack::unpack(packed.data(), packed.size());
std::vector<ILoggerChannel> logs2;
oh_vec.get().convert(logs2);




  //// SValueDt
  //inline std::vector<uint8_t> serialize_svaluedt(const SValueDt& val) {
  //  std::ostringstream buffer;
  //  msgpack::pack(buffer, val);
  //  std::string str = buffer.str();
  //  return std::vector<uint8_t>(str.begin(), str.end());
  //}

  //// ILoggerChannel
  //inline std::vector<uint8_t> serialize_logger_channel(const ILoggerChannel& val) {
  //  std::ostringstream buffer;
  //  msgpack::pack(buffer, val);
  //  std::string str = buffer.str();
  //  return std::vector<uint8_t>(str.begin(), str.end());
  //}

  //// IIdValueDtChannel
  //inline std::vector<uint8_t> serialize_id_value_dt_channel(const IIdValueDtChannel& val) {
  //  std::ostringstream buffer;
  //  msgpack::pack(buffer, val);
  //  std::string str = buffer.str();
  //  return std::vector<uint8_t>(str.begin(), str.end());
  //}

  //// IIdVecValueDtChannel
  //inline std::vector<uint8_t> serialize_id_vec_value_dt_channel(const IIdVecValueDtChannel& val) {
  //  std::ostringstream buffer;
  //  msgpack::pack(buffer, val);
  //  std::string str = buffer.str();
  //  return std::vector<uint8_t>(str.begin(), str.end());
  //}

  //// std::vector<SValueDt>
  //inline std::vector<uint8_t> serialize_svaluedt_vector(const std::vector<SValueDt>& vals) {
  //  std::ostringstream buffer;
  //  msgpack::pack(buffer, vals);
  //  std::string str = buffer.str();
  //  return std::vector<uint8_t>(str.begin(), str.end());
  //}

  //// std::vector<ILoggerChannel>
  //inline std::vector<uint8_t> serialize_logger_channel_vector(const std::vector<ILoggerChannel>& vals) {
  //  std::ostringstream buffer;
  //  msgpack::pack(buffer, vals);
  //  std::string str = buffer.str();
  //  return std::vector<uint8_t>(str.begin(), str.end());
  //}

  //// std::vector<IIdValueDtChannel>
  //inline std::vector<uint8_t> serialize_id_value_dt_channel_vector(const std::vector<IIdValueDtChannel>& vals) {
  //  std::ostringstream buffer;
  //  msgpack::pack(buffer, vals);
  //  std::string str = buffer.str();
  //  return std::vector<uint8_t>(str.begin(), str.end());
  //}

  //// std::vector<IIdVecValueDtChannel>
  //inline std::vector<uint8_t> serialize_id_vec_value_dt_channel_vector(const std::vector<IIdVecValueDtChannel>& vals) {
  //  std::ostringstream buffer;
  //  msgpack::pack(buffer, vals);
  //  std::string str = buffer.str();
  //  return std::vector<uint8_t>(str.begin(), str.end());
  //}

 */
