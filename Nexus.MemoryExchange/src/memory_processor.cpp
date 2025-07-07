// "MemoryProcessor.cpp"

#include "pch.h"

#include "MemoryExchange/memory_processor.h"

MemoryProcessor::MemoryProcessor(const std::string& name, server_client role, IMemoryDataHandler* handler)
  : handler_(handler) {
  if (!handler_) {
    throw std::invalid_argument("Handler (IMemoryDataHandler) cannot be nullptr.");
  }
  // Передаем лямбду, которая вызывает наш внутренний OnRawDataReceived
  memory_nome_ = std::make_unique<MemoryNome>(name, role,
    [this](const rec_data_meta_data& meta) {
      this->on_raw_data_received(meta);
    });
}

void MemoryProcessor::on_raw_data_received(const rec_data_meta_data& dMetaData) {
  // 1. Проверяем, является ли это подтверждением (ACK)
  const auto cmd_it = dMetaData.meta_data.find("command");
  if (cmd_it != dMetaData.meta_data.end() && cmd_it->second == "ok") {
    if (handler_) handler_->on_ack_received();
    return;
  }

  // 2. Если это не ACK, то это данные. Ищем ID типа.
  const auto type_it = dMetaData.meta_data.find("type");
  if (type_it == dMetaData.meta_data.end()) {
    std::cerr << "[MemoryProcessor] Received data without 'type' ID in metadata.\n";
    return;
  }

  try {
    uint32_t typeId = std::stoul(type_it->second);
    msgpack::object_handle oh = msgpack::unpack(reinterpret_cast<const char*>(dMetaData.bytes.data()), dMetaData.bytes.size());

    if (handler_) { // Проверяем, что есть обработчик
      // Используем switch для вызова соответствующего метода обработчика
      switch (typeId) {
      case e_logger:
        handler_->on_logger_data(oh.get().as<std::vector<IdLogger>>());
        break;
      case e_vector:
        handler_->on_vector_data(oh.get().as<std::vector<IdVector>>());
        break;
      case e_value:
        handler_->on_value_data(oh.get().as<std::vector<IdValue>>());
        break;
      case e_date_time_variable:
        handler_->on_date_time_variable_data(oh.get().as<std::vector<DateTimeVariable>>());
        break;
      case e_matrix:
        handler_->on_matrix_data(oh.get().as<std::vector<IdMatrix>>());
        break;
      case e_rec_result:
        handler_->on_rec_result_data(oh.get().as<std::vector<RecResult>>());
        break;
      //case e_dt_record:
      //  handler_->on_dt_record_data(oh.get().as<std::vector<DtRecord>>());
      //  break;
      default:
        std::cerr << "[MemoryProcessor] Received data with unknown type ID: " << typeId << "\n";
        break;
      }
    }
  }
  catch (const std::exception& e) {
    std::cerr << "[MemoryProcessor] Deserialization error: " << e.what() << '\n';
  }

  // ЭТО КЛЮЧЕВАЯ ДОРАБОТКА ДЛЯ СЕРВЕРА
  if (!dMetaData.meta_data.empty()) { // Очищаем только если были метаданные
    std::cout << "[MemoryProcessor] Данные обработаны, очищаю контрольный блок чтения...\n";
    clear_read_channel();
  }
}

metadata_map MemoryProcessor::check_write_channel() 
{
  if (!memory_nome_) return {};
  return memory_nome_->check_write_channel_control();
}

void MemoryProcessor::clear_read_channel() {
  if (!memory_nome_) return;
  memory_nome_->clear_read_channel_control();
}

