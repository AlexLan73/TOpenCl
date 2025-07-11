// ReSharper disable CppClangTidyClangDiagnosticInvalidUtf8
#pragma once

#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <functional>
#include <atomic>
#include <memory>
#include <optional>
#include "interfaces/i_memory_config_channel.h"
//#include "interfaces/rec_data_meta_data.h"
#include "MemoryExchange/my_msgpack_adapters.h" // Здесь находятся функции сериализации
#include "MemoryExchange/shared_data_types.h"

enum class RxMode : int {
  Default = 0,      // Передача по одному элементу
  BatchByCount = 1, // Батчинг по количеству
  BatchByTime = 2, // Батчинг по времени
  Average = 3  // Усреднение (например, для SValueDt)
};

class IChannelProcessor {
public:
  virtual ~IChannelProcessor() = default;
  virtual void dispose() = 0;
  virtual void set_params(const metadata_map& params) = 0;
  virtual void set_mode(RxMode mode) = 0;
};

// Универсальный шаблонный процессор
template<typename T>
class ChannelProcessor : public IChannelProcessor {
public:
  ChannelProcessor(std::shared_ptr<std::queue<rec_data_meta_data>> output_queue, 
    std::shared_ptr<std::mutex> mutex_transmitter,
    std::shared_ptr<std::condition_variable> transmitter_cv)
    : stop_flag_(false), rx_mode_(RxMode::Default), output_queue_(output_queue),
			mutex_transmitter_(mutex_transmitter), transmitter_cv_(transmitter_cv)
  {
    processing_thread_ = std::thread([this]() { this->run(); });
  }

//  ChannelProcessor() = default;

  ~ChannelProcessor() override {
    dispose();
  }

  void push(const T& data, const metadata_map& meta) {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      queue_.emplace(data, meta);
    }
    cv_.notify_one();
  }

  void set_params(const metadata_map& params) override {
    std::lock_guard<std::mutex> lock(mutex_);
    // Пример: batch_size, batch_time_ms, etc.
    if (auto it = params.find("batch_size"); it != params.end())
      batch_size_ = std::stoi(it->second);
    if (auto it = params.find("batch_time_ms"); it != params.end())
      batch_time_ms_ = std::stoi(it->second);
  }

  void set_mode(RxMode mode) override {
    std::lock_guard<std::mutex> lock(mutex_);
    rx_mode_ = mode;
  }

  void dispose() override {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      stop_flag_ = true;
    }
    cv_.notify_one();
    if (processing_thread_.joinable())
      processing_thread_.join();
  }

private:
  using QueueItem = std::pair<T, metadata_map>;
  std::queue<QueueItem> queue_;
  std::mutex mutex_;
  std::condition_variable cv_;

  std::shared_ptr<std::mutex> mutex_transmitter_;
  std::shared_ptr<std::condition_variable> transmitter_cv_;

  std::thread processing_thread_;
  std::atomic<bool> stop_flag_;
  RxMode rx_mode_;
  size_t batch_size_ = 10;
  size_t batch_time_ms_ = 750;
  std::shared_ptr<std::queue<rec_data_meta_data>> output_queue_;

  void run() {
    std::vector<QueueItem> batch;
    auto last_time = std::chrono::steady_clock::now();
    while (true) {
      QueueItem item;
      {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return stop_flag_ || !queue_.empty(); });
        if (stop_flag_ && queue_.empty())
          break;
        item = std::move(queue_.front());
        queue_.pop();
      }

      // Режимы RxMode
      switch (rx_mode_) {
      case RxMode::Default:
        process_single(item);
        break;
      case RxMode::BatchByCount:
        batch.push_back(item);
        if (batch.size() >= batch_size_) {
          process_batch(batch);
          batch.clear();
        }
        break;
      case RxMode::BatchByTime: {
        batch.push_back(item);
        auto now = std::chrono::steady_clock::now();
        if (static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time).count()) >=
          static_cast<int64_t>(batch_time_ms_))
//        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time).count() >= batch_time_ms_) 
        {
          process_batch(batch);
          batch.clear();
          last_time = now;
        }
        break;
      }
      case RxMode::Average:
        batch.push_back(item);
        if (batch.size() >= batch_size_) {
          process_average(batch);
          batch.clear();
        }
        break;
      }
    }
    // Обработка остатков
    if (!batch.empty()) {
      if (rx_mode_ == RxMode::BatchByCount || rx_mode_ == RxMode::BatchByTime)
        process_batch(batch);
      else if (rx_mode_ == RxMode::Average)
        process_average(batch);
    }
  }

  void process_single(const QueueItem& item) {
    rec_data_meta_data block;
    block.bytes = my_msgpack::serialize(item.first);
    block.meta_data = item.second;
    push_to_output(block);
  }

  void process_batch(const std::vector<QueueItem>& batch) {
    if (batch.empty()) return;
    std::vector<T> data_vec;
    metadata_map meta = batch.back().second;
    for (const auto& [data, _] : batch)
      data_vec.push_back(data);
    rec_data_meta_data block;
    block.bytes = my_msgpack::serialize(data_vec);
    block.meta_data = meta;
    push_to_output(block);
  }

  void process_average(const std::vector<QueueItem>& batch) {
    if constexpr (std::is_same_v<T, SValueDt>) {
      if (batch.empty()) return;
      double sum = 0;
      for (const auto& [data, _] : batch)
        sum += data.value;
      SValueDt avg;
      avg.value = sum / batch.size();
      avg.ticks = batch.back().first.ticks;
      rec_data_meta_data block;
      block.bytes = my_msgpack::serialize(avg);
      block.meta_data = batch.back().second;
      push_to_output(block);
    }
  }

  //void push_to_output(const rec_data_meta_data& block) {
  //  std::lock_guard<std::mutex> lock(mutex_);
  //  output_queue_->push(block);
  //}
  void push_to_output(const rec_data_meta_data& block) {
    std::lock_guard<std::mutex> lock(*mutex_transmitter_);
    output_queue_->push(block);
    transmitter_cv_->notify_one();
  }

};


