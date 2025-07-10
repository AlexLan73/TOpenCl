
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

class DataContext : public IDataContext {
public:
//  DataContext(std::shared_ptr<ILogger> log);
  DataContext();

  void send(int channel_type, const ILoggerChannel& data, const metadata_map& meta = {}) override;
  void send(int channel_type, const IIdValueDtChannel& data, const metadata_map& meta = {}) override;
  void send(int channel_type, const IIdVecValueDtChannel& data, const metadata_map& meta = {}) override;

  template <typename T>
  void register_channel(int channel_type) {
    std::lock_guard lock(mutex_);
    channels_[channel_type] = std::make_shared<ChannelProcessor<T>>(output_queue_);
  }

  void dispose();
  void send_logger(ILoggerChannel msg) override;
  void run_transmitter();

  // Очередь для передачи в C#
  std::shared_ptr<std::queue<rec_data_meta_data>> output_queue_;

private:
  std::mutex mutex_;
  std::mutex output_mutex_;
  std::condition_variable output_cv_;
  
  std::thread processing_thread_;
  std::map<int, std::shared_ptr<IChannelProcessor>> channels_;
  bool running_ = true;
  std::shared_ptr<ILogger> logger_;

  void initialization_channels();
};

