#include "pch.h"
#include "DataContracts/Protocol.h"

#include "DataContracts/IDataContext.h"


Protocol::Protocol(std::shared_ptr<TimeCounters> counters)
  : counters_(std::move(counters))
  {
    command_map_ = {
        {protocol_command::not_command, [this]() { std::cout << " Not command  \n"; }},
        {protocol_command::ok_command, [this]() { this->ok_command(); }},
        {protocol_command::work_ok_command, [this]() { std::cout << " Work OK Command  \n"; }},
        {protocol_command::disposable_command, [this]() {  std::cout << " Disposable Command  \n"; }}
    };

    event_map_ = {
        {event_command::not_event, [this]() { std::cout << " Not Event  \n"; }},
        {event_command::start_event, [this]() { std::cout << " Start Event  \n"; }},
        {event_command::stop_event, [this]() { std::cout << " Stop Event  \n"; }},
        {event_command::pause_event, [this]() { std::cout << " Pause Event   \n"; }},
        {event_command::reset_event, [this]() {  std::cout << " Reset Event   \n"; }}
    };

    mode_map_ = {
        {mode_command::not_mode, [this]() { std::cout << " Not Mode  \n"; }},
        {mode_command::initial_mode, [this]() { std::cout << " Initial Mode  \n"; }},
        {mode_command::work_mode, [this]() { std::cout << " Work Mode  \n"; }},
        {mode_command::disposable_mode, [this]() { std::cout << " Disposable Mode   \n"; }},
        {mode_command::restart_mode, [this]() {  std::cout << " Restart Mode   \n"; }}
    };
  }

  void Protocol::process_meta_data(const metadata_map& meta_data) {
    auto it = meta_data.find("command");
    if (it != meta_data.end()) 
    {
      //auto cmd_it = command_map_.find(it->second);
      //if (cmd_it != command_map_.end()) {
      //  cmd_it->second();
      //}
      //else {
      //  std::cout << "Unknown command: " << it->second << "\n";
      //}
    }
    else {
      std::cout << "No command in meta_data\n";
    }
  }

  void Protocol::reset_counters(){
    counters_->reset();
    std::cout << "Counters reset\n";
  }

  void Protocol::ok_command()
  {
    std::cout << "Ok command  \n";
  }

void Protocol::set_data_context(std::shared_ptr<IDataContext> data_context)
{
  data_context_ = data_context;
  data_context_->start();
}




