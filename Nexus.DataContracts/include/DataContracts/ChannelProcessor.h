// ReSharper disable CppClangTidyClangDiagnosticInvalidUtf8
#pragma once

#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <any>

class IChannelProcessor {
public:
  virtual ~IChannelProcessor() = default;
  virtual void dispose() = 0;
  virtual void set_params(const metadata_map & params) = 0;
};

template <typename T>
class ChannelProcessor : public IChannelProcessor {
public:
 // using MetadataMap = std::map<std::string, std::string>;

  ChannelProcessor() : stop_flag_(false) {
    processing_thread_ = std::thread([this]() { this->run(); });
  }

  ~ChannelProcessor() {
    dispose();
  }

  // Добавить данные в очередь
  void push(const T& data, const metadata_map& meta) {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      queue_.emplace(data, meta);
    }
    cv_.notify_one();
  }

  // Настроить параметры RxCpp (например, через set_params)
  void set_params(const metadata_map& params) override	{
    // Реализация настройки
  }

  // Остановить поток обработки
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
  std::thread processing_thread_;
  bool stop_flag_ = false;

  void run() {
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

//      process_rxcpp(item.first, item.second);
    }
  }

  void process_rxcpp(const T& data, const metadata_map& meta) {
    // Здесь можно реализовать RxCpp-обработку, агрегацию, формирование пакетов и т.д.
    // Например, отправить данные в RxCpp subject или observable
  }

  
  // RxCpp pipeline и параметры агрегации
  // ...

  void push_to_output(const rec_data_meta_data& block){
	  
  };


};
