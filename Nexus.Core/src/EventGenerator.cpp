#include "pch.h"
#include "Core/EventGenerator.h"

#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <map>
#include <atomic>
#include <ranges>

// Запуск генератора событий
void EventGenerator::start() {
  if (running_) return;
  running_ = true;
  paused_ = false;
  worker_ = std::thread([this]() {
    while (running_) {
      {
        std::unique_lock<std::mutex> lock(mutex_);
        if (paused_) {
          cv_.wait(lock, [this]() { return !paused_ || !running_; });
        }
        if (!running_) break;
      }
      // Копируем map callbacks для безопасного вызова вне lock
      std::map<int, Callback> callbacks_copy;
      {
        std::lock_guard<std::mutex> lock(mutex_);
        callbacks_copy = callbacks_;
      }
      for (auto& cb : callbacks_copy | std::views::values) {
        if (cb) cb();
      }
      std::this_thread::sleep_for(std::chrono::duration<double>(interval_));
    }
  });
}

// Остановка генератора
void EventGenerator::stop() {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
    paused_ = false;
  }
  cv_.notify_all();
  if (worker_.joinable()) {
    worker_.join();
  }
}

// Пауза
void EventGenerator::pause() {
  std::lock_guard<std::mutex> lock(mutex_);
  paused_ = true;
}

// Продолжить после паузы
void EventGenerator::resume() {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    paused_ = false;
  }
  cv_.notify_all();
}

// Полная остановка и очистка
void EventGenerator::dispose() {
  stop();
  std::lock_guard<std::mutex> lock(mutex_);
  callbacks_.clear();
}

// Подписка: по id (id задачи)
void EventGenerator::subscribe(int id, Callback cb) {
  std::lock_guard<std::mutex> lock(mutex_);
  callbacks_[id] = std::move(cb);
}

// Отписка по id
void EventGenerator::unsubscribe(int id) {
  std::lock_guard<std::mutex> lock(mutex_);
  callbacks_.erase(id);
}

// Проверить, есть ли подписка по id
bool EventGenerator::has_subscription(int id) const {
  std::lock_guard<std::mutex> lock(mutex_);
  return callbacks_.find(id) != callbacks_.end();
}

// Изменить интервал (сек)
void EventGenerator::set_interval(double interval_sec) {
  std::lock_guard<std::mutex> lock(mutex_);
  interval_ = interval_sec;
}
