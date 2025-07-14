
#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>
#include "ChannelProcessor.h"
#include "interfaces/i_memory_config_channel.h"

#include "IDataContext.h"
#include "Logger/ILogger.h"
#include "Protocol.h"


class DataContext : public IDataContext {
public:
  DataContext(const std::shared_ptr<ILogger>& i_logger, IProtocol& protocol);

  void send(int channel_type, const ILoggerChannel& data, const metadata_map& meta = {}) override;
  void send(int channel_type, const IIdValueDtChannel& data, const metadata_map& meta = {}) override;
  void send(int channel_type, const IIdVecValueDtChannel& data, const metadata_map& meta = {}) override;

  template <typename T>
  void register_channel(int channel_type) {
    std::lock_guard<std::mutex> lock(mutex_);
    channels_[channel_type] = std::make_shared<ChannelProcessor<T>>(output_queue_, output_mutex_, output_cv_);
  }

  void dispose();
  void send_logger(ILoggerChannel msg) override;
  void send_logger(const ILoggerChannel& data, const metadata_map& meta = {}) override
  {
    send(0, data, meta);
  }

  // Очередь для передачи в C#
  void run_transmitter();
  void start() override;

  std::shared_ptr<std::queue<rec_data_meta_data>> output_queue_;

  void on_memory_exchange_meta(const metadata_map& meta_data);
  void set_protocol(std::shared_ptr<IProtocol> protocol) override;
  
private:
  IProtocol& protocol_;
  std::mutex mutex_;

  std::shared_ptr<std::mutex> output_mutex_;
	std::shared_ptr<std::condition_variable> output_cv_;
  std::map<int, std::shared_ptr<IChannelProcessor>> channels_;
  std::shared_ptr<ILogger> i_logger_;

  std::thread processing_thread_;
  bool running_ = true;
  std::shared_ptr<IProtocol> protocol_new_;

  void initialization_channels();
};

