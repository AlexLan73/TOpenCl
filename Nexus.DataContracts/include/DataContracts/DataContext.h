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
	void send(std::string s) override;
	void send(const IVectorChannel vector_channel) override;
	void send(const IValueChannel value_channel) override;

	void addTask(std::function<void()> task) override;

	void send_logger(ILoggerChannel msg) override;

  template <typename T>
  void register_channel(int channel_type) {
    std::lock_guard<std::mutex> lock(mutex_);
    channels_[channel_type] = std::make_shared<ChannelProcessor<T>>();
  }

  //template <typename T>
  //void send(int channel_type, const T& data, const std::map<std::string, std::string>& meta = {}) {
  //  std::lock_guard<std::mutex> lock(mutex_);
  //  auto it = channels_.find(channel_type);
  //  if (it != channels_.end()) {
  //    std::static_pointer_cast<ChannelProcessor<T>>(it->second)->push(data, meta);
  //  }
  //}

  template <typename T>
  void send(int channel_type, const T& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    metadata_map meta = metadata_map_base_;
    meta[name_type_] = std::to_string(channel_type);
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

  void send_channel(std::vector<ILoggerChannel>& data) override;
  void send_channel(std::vector<IDateTimeVariableChannel>& data) override;
  void send_channel(std::vector<IVectorChannel>& data) override;
  void send_channel(std::vector<IMatrixChannel>& data) override;
  void send_channel(std::vector<IRecResultChannel>& data) override;
  void send_channel(std::vector<IValueChannel>& data) override;


private:
  void initialization_channels();
  std::mutex mutex_;
  std::map<int, std::shared_ptr<IChannelProcessor>> channels_; // int — тип канала, void — универсальный контейнер
  metadata_map metadata_map_base_;
  const std::string name_type_ = "type";
//	std::map<std::string, std::string> m_base;
//  m_base["type"] = "-999";

  // Клонируем m_base в m_clone
//  std::map<std::string, std::string> m_clone = m_base;
};

