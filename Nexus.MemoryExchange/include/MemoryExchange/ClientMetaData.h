#pragma once
#include <windows.h>
#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <chrono>
#include <unordered_map>

#include "BasicMemoryMd.h"
#include "interfaces/i_memory_config_channel.h"
#include "MetaSettings.h"
#include "interfaces/MdCommand.h"

class ClientMetaData
{
  // BasicMemoryMd(const std::string& eventName, int size, const std::string& controlName,
  //               std::function<void(const MapCommands&)> callBack, HANDLE sendTo);
public:
  MetaSettings meta_settings;
  BasicMemoryMd* md;
  HANDLE send_to_server;
  std::atomic<bool> stop_requested;
  std::thread wait_thread;
  std::function<void(const metadata_map&)> call_back;
  std::string name_module;

  ClientMetaData(const MetaSettings& meta_settings);

  ~ClientMetaData();
  void dispose();
  static void PrintMapClient(const metadata_map& map);
  void work_dispose();


private:
  void ReadDataCallBack();
  void OnMetaData(const metadata_map& map);
  SateMode _mode;
};

