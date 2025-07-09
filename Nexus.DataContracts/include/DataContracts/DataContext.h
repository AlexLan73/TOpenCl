#pragma once
#include <mutex>
#include <ranges>

#include "IDataContext.h"
#include "MemoryExchange/memory_nome.h"
#include "MemoryExchange/i_memory_data_handler.h"
#include "ChannelProcessor.h"

class DataContext:public IDataContext
{
public:
	DataContext();
//	void send(std::string s) override;
	//void send(const IVectorChannel vector_channel) override;
	//void send(const IValueChannel value_channel) override;

//	void addTask(std::function<void()> task) override;

	void send_logger(ILoggerChannel msg) override;

  template <typename T>
  void register_channel(int channel_type) {
    std::lock_guard<std::mutex> lock(mutex_);
    channels_[channel_type] = std::make_shared<ChannelProcessor<T>>();
  }

  void send(const int channel_type, const ILoggerChannel& data, const metadata_map& meta = {}) override {
    send<ILoggerChannel>(channel_type, data, meta);
  }
  void send(int channel_type, const IIdValueDtChannel& data, const metadata_map& meta = {}) override {
    send<IIdValueDtChannel>(channel_type, data, meta);
  }
  void send(int channel_type, const IIdVecValueDtChannel& data, const metadata_map& meta = {}) override {
    send<IIdVecValueDtChannel>(channel_type, data, meta);
  }

  template <typename T>
  void send(int channel_type, const T& data, const metadata_map& meta = {}) {
    std::lock_guard<std::mutex> lock(mutex_);
//    metadata_map meta = metadata_map_base_;
    auto it = channels_.find(channel_type);
    if (it != channels_.end()) {
      std::static_pointer_cast<ChannelProcessor<T>>(it->second)->push(data, meta);
    }
  }

  void dispose() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& processor : channels_ | std::views::values) {
      processor->dispose();
    }
  }

  //  !!!!!!!  
  void send_channel(std::vector<ILoggerChannel>& data, metadata_map& meta);
  void send_channel(std::vector<IIdValueDtChannel>& data, metadata_map& meta);
  void send_channel(std::vector<IIdVecValueDtChannel>& data, metadata_map& meta);

private:
  void initialization_channels();
  std::mutex mutex_;
  std::map<int, std::shared_ptr<IChannelProcessor>> channels_; // int — тип канала, void — универсальный контейнер
  metadata_map metadata_map_base_;
  const std::string name_type_ = "type";
//  тип канала
  std::queue<rec_data_meta_data> output_queue_; // очередь для передачи 

//	std::map<std::string, std::string> m_base;
//  m_base["type"] = "-999";

  // Клонируем m_base в m_clone
//  std::map<std::string, std::string> m_clone = m_base;
};

