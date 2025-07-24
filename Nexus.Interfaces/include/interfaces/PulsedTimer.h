// ReSharper disable CppClangTidyClangDiagnosticInvalidUtf8
#pragma once
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>

class ITimerEventListener {
public:
  virtual ~ITimerEventListener() = default;
  virtual void On250ms() {}
  virtual void On1Sec() {}
  virtual void On5Sec() {}
};


class PulseTimer {
public:
  static PulseTimer& Instance() {
    static PulseTimer inst;
    return inst;
  }

  void Subscribe(ITimerEventListener* listener) {
    std::lock_guard<std::mutex> lock(mtx_);
    listeners_.push_back(listener);
  }

  void Unsubscribe(ITimerEventListener* listener) {
    std::lock_guard<std::mutex> lock(mtx_);
    listeners_.erase(std::remove(listeners_.begin(), listeners_.end(), listener), listeners_.end());
  }

  void Start() {
    running_.store(true);
    worker_ = std::thread(&PulseTimer::Run, this);
  }

  void Stop() {
    running_.store(false);
    if (worker_.joinable()) worker_.join();
  }

private:
  PulseTimer() : running_(false) {}
  ~PulseTimer() { Stop(); }

  void Run() {
    int ticks = 0;
    while (running_.load()) {
      {
        // Копируем список подписчиков под мьютексом
        std::vector<ITimerEventListener*> copy;
        {
          std::lock_guard<std::mutex> lock(mtx_);
          copy = listeners_;
        }
        // 0.25 сек событие
        for (auto* l : copy) l->On250ms();
        // 1 сек (каждые 4 тика)
        if (ticks % 4 == 0) for (auto* l : copy) l->On1Sec();
        // 5 сек (каждые 20 тиков)
        if (ticks % 20 == 0) for (auto* l : copy) l->On5Sec();
      }
      ticks = (ticks + 1) % 1000000;
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
  }

  std::atomic<bool> running_;
  std::thread worker_;
  std::mutex mtx_;
  std::vector<ITimerEventListener*> listeners_;
};
