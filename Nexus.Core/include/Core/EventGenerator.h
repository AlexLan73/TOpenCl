#pragma once

#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <map>

class EventGenerator {
public:
  using Callback = std::function<void()>;

  explicit EventGenerator(double interval_sec = 0.5)
    : interval_(interval_sec), running_(false), paused_(false) {
  }

  // Запуск генератора событий
  void start();

  // Остановка генератора
  void stop();

  // Пауза
  void pause();

  // Продолжить после паузы
  void resume();

  // Полная остановка и очистка
  void dispose();

  // Подписка: по id (id задачи)
  void subscribe(int id, Callback cb);

  // Отписка по id
  void unsubscribe(int id);

  // Проверить, есть ли подписка по id
  bool has_subscription(int id) const;

  // Изменить интервал (сек)
  void set_interval(double interval_sec);

  ~EventGenerator() {
    dispose();
  }

private:
  double interval_;
  std::atomic<bool> running_;
  bool paused_;
  std::thread worker_;
  mutable std::mutex mutex_;
  std::condition_variable cv_;
  std::map<int, Callback> callbacks_;
};
