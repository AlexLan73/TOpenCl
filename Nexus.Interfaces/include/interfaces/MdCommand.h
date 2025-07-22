// ReSharper disable CppClangTidyClangDiagnosticInvalidUtf8
#pragma once
#include <string>

// 1. Перечисление MdCommand
enum class MdCommand {
  None,
  State,
  Command,
  Work,
  Ok,
  No,
  Error,
  Crc
};

// Функция для преобразования MdCommand в строку (AsKey)
inline std::string AsKey(MdCommand cmd) {
  switch (cmd) {
  case MdCommand::State:   return "state";
  case MdCommand::Command: return "command";
  case MdCommand::Work:    return "work";
  case MdCommand::Ok:      return "ok";
  case MdCommand::No:      return "no";
  case MdCommand::Error:   return "error";
  case MdCommand::Crc:     return "crc";
  default:                 return "";
  }
}

// 2. Перечисление SateMode
enum class SateMode {
  None,
  Initialization,
  Work,
  Dispose
};

inline std::string AsKey(SateMode cmd) {
  switch (cmd) {
  case SateMode::None: return "None";
  case SateMode::Initialization: return "Initialization";
  case SateMode::Work: return "Work";
  case SateMode::Dispose: return "Dispose";
  default:                   return "";
  }
}

// 3. Перечисление ClientServer
enum class ClientServer {
  Client,
  Server
};

// Функция для преобразования ClientServer в строку (AsKey)
inline std::string AsKey(ClientServer cmd) {
  switch (cmd) {
  case ClientServer::Client: return "client";
  case ClientServer::Server: return "server";
  default:                   return "";
  }
}

enum  TransferWaiting
{
  None,
  Transfer,
  Waiting
};
