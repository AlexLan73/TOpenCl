// ReSharper disable CppClangTidyClangDiagnosticCoveredSwitchDefault
#include "pch.h"
#include "DataContracts/DataContext.h"
#include "interfaces/i_memory_config_channel.h"
#include <iostream>
#include <ranges>


DataContext::DataContext(const std::shared_ptr<ILogger>& i_logger, IProtocol& protocol)
    : i_logger_(i_logger),  protocol_(protocol)
{
	std::cerr << "  Start DataContext constructor " << '\n';
  output_queue_ = std::make_shared<std::queue<rec_data_meta_data>>();
  output_mutex_ = std::make_shared<std::mutex>();
  output_cv_ = std::make_shared<std::condition_variable>();
  initialization_channels();
  processing_thread_ = std::thread([this]() { this->run_transmitter(); });

}

void DataContext::on_memory_exchange_meta(const std::map<std::string, std::string>& meta_data) {
  protocol_.process_meta_data(meta_data);
}

void DataContext::set_protocol(std::shared_ptr<IProtocol> protocol) 
{
  protocol_new_ = protocol;
  protocol_new_->reset_counters();
}

void DataContext::dispose()
{
  // 1. Остановить все процессоры каналов
  for (auto& processor : channels_ | std::views::values) {
    if (processor) {
      processor->dispose();
    }
  }

	// 2. Очистить карту каналов
  channels_.clear();

  // 3. Очистить очередь для передачи данных (если требуется)
  if (output_queue_) {
    std::queue<rec_data_meta_data> empty;
    std::swap(*output_queue_, empty);
  }
}

void DataContext::send_logger(ILoggerChannel msg)
{
}

void DataContext::initialization_channels()
{

  for (uint32_t i = e_logger; i < COUNT_TYPE_DATA_CHANNEL; ++i) {
    switch (static_cast<data_type_ids>(i)) {
    case e_logger:
      register_channel<ILoggerChannel>(0);     // Логи
      std::cout << "e_logger\n";
      break;
    case e_id_value_dt_channel:
      register_channel<IIdValueDtChannel>(1);     // Логи
      std::cout << "e_id_value_dt_channel \n";
      break;
    case e_id_vec_value_dt_channel:
      register_channel<IIdVecValueDtChannel>(2);     // Логи
      std::cout << "e_id_vec_value_dt_channel \n";
      break;

    default:
      std::cout << "Unknown value\n";
      break;
    }
  }

}

void DataContext::run_transmitter() {
  while (running_) {
    rec_data_meta_data block;
    {
      std::unique_lock<std::mutex> lock(*output_mutex_);
      output_cv_->wait(lock, [this] { return !output_queue_->empty() || !running_; });
      if (!running_ && output_queue_->empty()) break;
      block = output_queue_->front();
      output_queue_->pop();
    }
    // Передача данных в C#
//    transmit_to_csharp(block);
  }
}

void DataContext::start()
{
  ILoggerChannel log1{ 0, "Start DataContext", " Di  test!!! ", logger_send_enum_memory::info };
  i_logger_->log(log1);

}


void DataContext::send(int channel_type, const ILoggerChannel& data, const metadata_map& meta ) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = channels_.find(channel_type);
  if (it != channels_.end()) {
    // Приведение к нужному типу ChannelProcessor
    std::static_pointer_cast<ChannelProcessor<ILoggerChannel>>(it->second)->push(data, meta);
  }
}

void DataContext::send(int channel_type, const IIdValueDtChannel& data, const metadata_map& meta ) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = channels_.find(channel_type);
  if (it != channels_.end()) {
    std::static_pointer_cast<ChannelProcessor<IIdValueDtChannel>>(it->second)->push(data, meta);
  }
}

void DataContext::send(int channel_type, const IIdVecValueDtChannel& data, const metadata_map& meta )  {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = channels_.find(channel_type);
  if (it != channels_.end()) {
    std::static_pointer_cast<ChannelProcessor<IIdVecValueDtChannel>>(it->second)->push(data, meta);
  }
}

