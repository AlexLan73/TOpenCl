// ReSharper disable CppClangTidyClangDiagnosticInvalidUtf8
#include "pch.h"
#include <unordered_map>
#include "MemoryExchange/ClientMetaData.h"
#include "interfaces/MdCommand.h"


ClientMetaData::ClientMetaData(MetaSettings& meta_settings, const std::shared_ptr<ServerMetaDataTimer> md_time)
  : meta_settings(meta_settings), md(nullptr),
    send_to_server(nullptr),
    stop_requested(false),
    call_back(nullptr),
    _mode(SateMode::Initialization),
    md_time_(md_time)
{
  md_time->reset();

  name_module = "client" + meta_settings.GetMemoryName();

  // Создаем/открываем событие для отправки серверу
  send_to_server = CreateEventA(nullptr, FALSE, FALSE, meta_settings.GetMetaEventServer().c_str());
  if (!send_to_server) throw std::runtime_error("Failed to create sendToServer event");

  // Создаем вспомогательный объект для работы с памятью
  md = new BasicMemoryMd(
    meta_settings.GetMetaEventClient(),
    meta_settings.GetMetaSize(),
    meta_settings.GetControlName(),
    [this](const metadata_map& map) { this->OnMetaData(map); },
    send_to_server
  );
  // Запускаем поток
  metadata_map ack = {
    { AsKey(MdCommand::State), name_module },
  };
  md->WriteMetaMap(ack);
  _transferWaiting = TransferWaiting::None;


//  wait_thread = std::thread(&ClientMetaData::ReadDataCallBack, this);
}

ClientMetaData::~ClientMetaData() {
  dispose();
}

void ClientMetaData::dispose() {
  stop_requested = true;
  if (wait_thread.joinable()) {
    SetEvent(send_to_server);
    wait_thread.join();
  }
  if (md) { md->Dispose(); delete md; md = nullptr; }
  if (send_to_server) { CloseHandle(send_to_server); send_to_server = nullptr; }
}

void ClientMetaData::WriteMetaMap(const metadata_map& ack) const
{
  md->WriteMetaMap(ack);
}

void ClientMetaData::OnMetaData(const metadata_map& map) {
  if (map.empty()) return;
  const auto state_it = map.find(AsKey(MdCommand::State));
  // название такое же выходим
  if (state_it == map.end() || state_it->second == name_module) return;

  reset_timeGeneralWorke();

  switch (_mode)
  {
  case SateMode::Initialization:
  {
    const auto cmd_it = map.find(AsKey(MdCommand::Command));
    if (cmd_it == map.end())
    {
      metadata_map ack = {
        { AsKey(MdCommand::State), name_module },
        { AsKey(MdCommand::Command), "_" }
      };
      _transferWaiting = TransferWaiting::Transfer;
      md->WriteMetaMap(ack);
      return;
    }
    
	// Если команд нет — шлём пустое подтверждение
      metadata_map ack = {
             { AsKey(MdCommand::State), name_module },
             { AsKey(MdCommand::Command), AsKey(MdCommand::Ok) }
      };
      _transferWaiting = TransferWaiting::Transfer;
      _mode = SateMode::Work;
      md->WriteMetaMap(ack);
    break;
  }
  case SateMode::Work:
  {
    // Здесь будет основная логика работы: приём данных, реакции, управление
    // 👇 Пока ничего не шлём, ждём команды подтверждения
    //  Когда будут посылаться данные ставится TransferWaiting.Waiting !!
    // Здесь будет основная логика работы: приём данных, реакции, управление
    std::cout <<  " READ DATA in CLIENT Work" << '\n';
    // 👇 Пока ничего не шлём, ждём команды подтверждения
    if(map.size()<2) return;
    reset_timeWork();
    reset_timeGeneralWorke();
    const auto cmd_it = map.find(AsKey(MdCommand::Command));
    if (cmd_it != map.end())
    {
      if (cmd_it->second == AsKey(MdCommand::Ok))
      {
        _transferWaiting = TransferWaiting::Transfer; // подтверждение, что данные были приняты
        reset_timeInitialization();
        std::cout << ">>> [CLIENT]  Work data OK!!!  " << '\n';
        return;
      } else
      {
        // Ключ есть, но значение не Ok
        // Здесь можно обработать эту ситуацию или просто идти дальше      
				//  Пока не понятно
      }
    }else
    {
      // Ключ отсутствует в map.
      // Обработать отсутствие ключа: false, error и т.п.

      std::vector<std::string> searchTerms1 = { AsKey(MdCommand::State), "id" };
      std::vector<std::string> keys;
      for (const auto& kv : map) {
        keys.push_back(kv.first);
      }
      // лямбда-функция, которая возвращает все ключи, содержащие хотя бы одну из поисковых подстрок
      auto find_matched_keys = [&](const std::vector<std::string>& searchTerms) -> std::vector<std::string> {
        std::vector<std::string> matchedKeys;
        for (const auto& key : keys) {
          // проверяем, содержится ли хотя бы одна подстрока в key
          bool matched = false;
          for (const auto& term : searchTerms) {
            if (contains_ignore_case(key, term)) {
              matched = true;
              break;
            }
          }
          if (matched) {
            matchedKeys.push_back(key);
          }
        }
        return matchedKeys;
        };

    	auto result1 = find_matched_keys(searchTerms1);
      if(result1.empty())
        return;

      ////if()  РАЗБОР КОММАНД ДЛЯ СЧИТЫВАНИЯ
      auto print_vector = [](const std::vector<std::string>& vec) {
        std::cout << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
          std::cout << vec[i];
          if (i + 1 < vec.size()) std::cout << ", ";
        }
        std::cout << "]";
      };
      print_vector(result1);
      metadata_map ack = {
        { AsKey(MdCommand::State), name_module },
        { AsKey(MdCommand::Command), AsKey(MdCommand::Ok)}
      };
      _transferWaiting = TransferWaiting::Transfer;
      md->WriteMetaMap(ack);
			return;
    }
    break;
  }

  case SateMode::Dispose:
  {

    break;
  }
  case SateMode::None:
  default:;
  }


  const auto cmd_it = map.find(AsKey(MdCommand::Command));

  if (_mode == SateMode::Initialization &&
    state_it->second.find("server") != std::string::npos &&
    cmd_it != map.end() &&
    cmd_it->second == AsKey(MdCommand::Ok))
  {
    _mode = SateMode::Work;
    std::cout << "Handshake завершён, переходим в режим Work" << '\n';
    return;
  }

  if (_mode == SateMode::Work &&
    state_it->second.find("server") != std::string::npos)
  {
    // Здесь обработка данных:
    PrintMapClient(map);
    // Подтверждаем серверу -> "ок" всегда только после анализа!
    metadata_map ack = {
        { AsKey(MdCommand::State), name_module },
        { AsKey(MdCommand::Command), AsKey(MdCommand::Ok) }
    };
    md->WriteMetaMap(ack);
    std::cout << "[client] Ответил serverCUDA: ok\n";
  }
}

bool ClientMetaData::contains_ignore_case(const std::string& text, const std::string& pattern) {
  auto it = std::search(
    text.begin(), text.end(),
    pattern.begin(), pattern.end(),
    [](char ch1, char ch2) { return std::tolower(ch1) == std::tolower(ch2); }
  );
  return (it != text.end());
}

void ClientMetaData::On250ms()
{
  // inc
  if (_mode == SateMode::Work)
    ++_timeWork;
  else
    reset_timeWork();

  // блок Comparison
  if (_mode == SateMode::Work && _timeWork > compare_work)
  {
    reset_timeWork();
    metadata_map ack = {
      { AsKey(MdCommand::State), name_module },
      { AsKey(MdCommand::Command), "_" }
    };
    _transferWaiting = TransferWaiting::Transfer;
    md->WriteMetaMap(ack);
    ++_workSendCount;
  }
}

void ClientMetaData::On1Sec()
{
	// inc
  if (_mode == SateMode::Initialization)
    ++_timeInitialization;
  else
    reset_timeInitialization();
  // блок Comparison

  if (_mode == SateMode::Work 
    && _timeInitialization > compare_initialization_)
  { // время вышло связи нет переходим на начальный уровень
    _mode = SateMode::Initialization;
    _transferWaiting = TransferWaiting::None;
    reset_timeWork();
  	reset_timeInitialization();
    metadata_map ack = {
      { AsKey(MdCommand::State), name_module },
      { AsKey(MdCommand::Command), "_" }
    };
    md->WriteMetaMap(ack);
    return;
  }

  if (_mode == SateMode::Initialization && (_timeInitialization % 5 == 1))
  { // время вышло связи нет переходим на начальный уровень
//    _mode = SateMode::Initialization;
    _transferWaiting = TransferWaiting::None;
    reset_timeWork();
    //      ResetInitialization();
    metadata_map ack = {
      { AsKey(MdCommand::State), name_module },
      { AsKey(MdCommand::Command), "_" }
    };
    md->WriteMetaMap(ack);
    return;
  }

  if (_mode == SateMode::Work && _workSendCount > compare_work_send_count_)
  {
    _mode = SateMode::Initialization;
    reset_timeInitialization();
    reset_timeWork();
    reset_workSendCount();

  	metadata_map ack = {
      { AsKey(MdCommand::State), name_module },
      { AsKey(MdCommand::Command), "_" }
    };
    md->WriteMetaMap(ack);
    ++_workSendCount;
  }
}


void ClientMetaData::On5Sec()
{
	// inc
  if (_mode == SateMode::Initialization)
    ++_timeGeneralWork;
  else
    reset_timeGeneralWorke();
  // блок Comparison


}


void ClientMetaData::PrintMapClient(const metadata_map& map) {
  for (const auto& [fst, snd] : map) {
    std::cout << " - GLOBAL!!! client ==> " << fst << " = " << snd << "\n";
  }
}

void ClientMetaData::work_dispose() {
  _mode = SateMode::Dispose;
  stop_requested = true;
  if (wait_thread.joinable()) wait_thread.join();
}


//void ClientMetaData::ReadDataCallBack() {
//  try {
//    while (!stop_requested) {
//      if (_mode == SateMode::Initialization) {
//        metadata_map init = {
//            { AsKey(MdCommand::State), name_module },
//            { AsKey(MdCommand::Command), "_" }
//        };
//        md->WriteMetaMap(init);
//        std::this_thread::sleep_for(std::chrono::milliseconds(400));
//      }
//      else if (_mode == SateMode::Work) {
//        // Просто ждем — реакции только через OnMetaData
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//      }
//      else if (_mode == SateMode::Dispose) {
//        metadata_map fin = {
//            { AsKey(MdCommand::State), name_module },
//            { AsKey(MdCommand::Command), AsKey(MdCommand::Ok) }
//        };
//        md->WriteMetaMap(fin);
//        break;
//      }
//    }
//  }
//  catch (const std::exception& ex) {
//    std::cerr << "Error in ReadDataCallBack: " << ex.what() << "\n";
//    if (call_back) call_back(metadata_map{});
//  }
//}
//

//
///*
//ClientMetaData::ClientMetaData(const MetaSettings& metaSettings)
//			: _metaSettings(metaSettings),
//				md(nullptr),
//				sendToServer(nullptr),
//				_stopRequested(false),
//				_callBack(nullptr),
//        _mode(SateMode::Initialization)
//{
//  _nameModule = "client" + _metaSettings.GetMemoryName();
//
//  // Создаем/открываем событие для посылки серверу
//  sendToServer = CreateEventA(nullptr, 
//    FALSE, 
//    FALSE, 
//    _metaSettings.GetMetaEventServer().c_str());
//  if (!sendToServer)  throw std::runtime_error("Failed to create sendToServer event");
//
//  // Создаём экземпляр BasicMemoryMd
//  md = new BasicMemoryMd(_metaSettings.GetMetaEventClient(), 
//												_metaSettings.GetMetaSize(), 
//												_metaSettings.GetControlName(),
////    [this](const metadata_map& map) { ClientMetaData::PrintMapClient(map); },
//[this](const metadata_map& map) { this->PrintMapClient(map); },
//    sendToServer);
//
//  // Запускаем поток работы
//  _waitThread = std::thread(&ClientMetaData::ReadDataCallBack, this);
//
//}
//
//ClientMetaData::~ClientMetaData()
//{
//  dispose();
//}
//
//void ClientMetaData::dispose()
//{
//  std::cout << "ClientPing  -- Dispose() \n";
//  _stopRequested = true;
//
//  if (_waitThread.joinable()) {
//    // Чтобы прервать ожидание в BasicMemoryMd, посылаем событие
//    SetEvent(sendToServer);
//    _waitThread.join();
//  }
//
//  if (md) {
//    md->Dispose();
//    delete md;
//    md = nullptr;
//  }
//
//  if (sendToServer) {
//    CloseHandle(sendToServer);
//    sendToServer = nullptr;
//  }
//
//}
//
//void ClientMetaData::ReadDataCallBack(){
//  int i = 0;
//  try {
//    // Отправляем первый "pong"
//    metadata_map map1 = {
//        { AsKey(MdCommand::State), _nameModule },
//        { "id_client", std::to_string(i) }
//    };
//    md->WriteMetaMap(map1);
//    // sendToServer сигналить не нужно, WriteMetaMap делает это
//
//    while (!_stopRequested) {
//      std::this_thread::sleep_for(std::chrono::seconds(1));
//      std::cout << "ClientPing  i= "<<  i <<"\n";
//
//      metadata_map map = {
//          { AsKey(MdCommand::State), _nameModule },
//          { "id_client", std::to_string(i) },
//						{AsKey(MdCommand::Command), "_"},
//      };
//      md->WriteMetaMap(map);
//
//      i++;
//    }
//  }
//  catch (const std::exception& ex) {
//    std::cerr << "Error in ReadDataCallBack: " << ex.what() << "\n";
//    if (_callBack) _callBack(metadata_map{}); // аналог _callBack(null)
//  }
//
//}
//
//void ClientMetaData::PrintMapClient(const metadata_map& map)
//{
//	for (const auto& [fst, snd] : map) {
//		std::cout << " - GLOBAL!!! client == >  " << fst << " = " << snd << "\n";
//	}
//
//}
//
//void ClientMetaData::work_dispose()
//{
//  {
//    _stopRequested = true;
//    _waitThread.join();
//  }
//
//}
//*/
