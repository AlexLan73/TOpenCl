#pragma once

#include <map>
#include <string>
#include <functional>
#include <iostream>

#include <memory>
#include "interfaces/TimeCounters.h"

#include "interfaces/kit_enums.h"
#include "interfaces/i_memory_config_channel.h"

#include <boost/di.hpp>
namespace di = boost::di;

class IProtocol {
public:
  virtual ~IProtocol() = default;
  virtual void process_meta_data(const metadata_map& meta_data) = 0;
  virtual void reset_counters() = 0;
};

class Protocol : public IProtocol {
public:
  Protocol(std::shared_ptr<TimeCounters> counters);

  void process_meta_data(const metadata_map& meta_data) override;
  void reset_counters() override;
  void ok_command();

private:
  std::shared_ptr<TimeCounters> counters_;
  std::map<protocol_command, std::function<void()>> command_map_;
  std::map<event_command, std::function<void()>> event_map_;
  std::map<mode_command, std::function<void()>> mode_map_;

  protocol_command protocol_current_ = protocol_command::not_command;
  event_command event_current_ = event_command::not_event;
  mode_command mode_current_ = mode_command::not_mode;

};

/*

enum protocol_command {
  not_command = 0,
  ok_command = 1,
  work_ok_command = 2,
  disposable_command = 3
};

// добавляем команды

enum event_command {
  not_event = 0,
  start_event = 1,
  stop_event = 2,
  pause_event = 3,
  reset_event = 4
};

enum mode_command {
  not_mode = 0,
  initial_mode = 1,
  work_mode = 2,
  disposable_mode = 3,
  restart_mode = 4
};



 */