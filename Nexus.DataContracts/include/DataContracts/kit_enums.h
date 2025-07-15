#pragma once
enum protocol_command {
  not_command = 0,
  ok_command = 1,
  work_ok_command = 2,
  disposable_command = 3
};

// добавляем команды 

enum event_command {
  not_event =0,
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

