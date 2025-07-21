// ReSharper disable CppClangTidyClangDiagnosticInvalidUtf8
#include "pch.h"
#include "MemoryExchange/ClientMetaData.h"

#include "interfaces/MdCommand.h"

ClientMetaData::ClientMetaData(const MetaSettings& metaSettings)
			: _metaSettings(metaSettings),
				md(nullptr),
				sendToServer(nullptr),
				_stopRequested(false),
				_callBack(nullptr)
{
  _nameModule = "client" + _metaSettings.GetMemoryName();

  // Создаем/открываем событие для посылки серверу
  sendToServer = CreateEventA(nullptr, 
    FALSE, 
    FALSE, 
    _metaSettings.GetMetaEventServer().c_str());
  if (!sendToServer) {
    throw std::runtime_error("Failed to create sendToServer event");
  }

  // Создаём экземпляр BasicMemoryMd
  md = new BasicMemoryMd(_metaSettings.GetMetaEventClient(), 
												_metaSettings.GetMetaSize(), 
												_metaSettings.GetControlName(),
//    [this](const metadata_map& map) { ClientMetaData::PrintMapClient(map); },
[this](const metadata_map& map) { this->PrintMapClient(map); },
    sendToServer);

  // Запускаем поток работы
  _waitThread = std::thread(&ClientMetaData::ReadDataCallBack, this);

}

ClientMetaData::~ClientMetaData()
{
  dispose();
}

void ClientMetaData::dispose()
{
  std::cout << "ClientPing  -- Dispose() \n";
  _stopRequested = true;

  if (_waitThread.joinable()) {
    // Чтобы прервать ожидание в BasicMemoryMd, посылаем событие
    SetEvent(sendToServer);
    _waitThread.join();
  }

  if (md) {
    md->Dispose();
    delete md;
    md = nullptr;
  }

  if (sendToServer) {
    CloseHandle(sendToServer);
    sendToServer = nullptr;
  }

}

void ClientMetaData::ReadDataCallBack(){
  int i = 0;
  try {
    // Отправляем первый "pong"
    metadata_map map1 = {
        { AsKey(MdCommand::State), _nameModule },
        { "id_client", std::to_string(i) }
    };
    md->WriteMetaMap(map1);
    // sendToServer сигналить не нужно, WriteMetaMap делает это

    while (!_stopRequested) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << "ClientPing  i= "<<  i <<"\n";

      metadata_map map = {
          { AsKey(MdCommand::State), _nameModule },
          { "id_client", std::to_string(i) },
						{AsKey(MdCommand::Command), "_"},
      };
      md->WriteMetaMap(map);

      i++;
    }
  }
  catch (const std::exception& ex) {
    std::cerr << "Error in ReadDataCallBack: " << ex.what() << "\n";
    if (_callBack) _callBack(metadata_map{}); // аналог _callBack(null)
  }

}

void ClientMetaData::PrintMapClient(const metadata_map& map)
{
	for (const auto& [fst, snd] : map) {
		std::cout << " - GLOBAL!!! client == >  " << fst << " = " << snd << "\n";
	}

}

void ClientMetaData::work_dispose()
{
  {
    _stopRequested = true;
    _waitThread.join();
  }

}
