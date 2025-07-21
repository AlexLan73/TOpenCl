#pragma once
#include <windows.h>
#include <string>
#include <iostream>
#include <thread>
#include <atomic>
#include <functional>
#include <chrono>
#include <unordered_map>

#include "BasicMemoryMd.h"
#include "interfaces/i_memory_config_channel.h"
#include "MetaSettings.h"

class ClientMetaData
{
  // BasicMemoryMd(const std::string& eventName, int size, const std::string& controlName,
  //               std::function<void(const MapCommands&)> callBack, HANDLE sendTo);
public:
  MetaSettings _metaSettings;
  BasicMemoryMd* md;
  HANDLE sendToServer;
  std::atomic<bool> _stopRequested;
  std::thread _waitThread;
  std::function<void(const metadata_map&)> _callBack;
  std::string _nameModule;

  ClientMetaData(const MetaSettings& metaSettings);

  ~ClientMetaData();
  void dispose();
  void PrintMapClient(const metadata_map& map);
  void work_dispose();


private:
  void ReadDataCallBack();

};

