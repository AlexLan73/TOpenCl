#pragma once
#include <conio.h>
#include <memory>
#include <string>

#include <iostream>   // Для std::cout
#include <string>     // Для std::string
#include <chrono>     // Для std::chrono::system_clock, std::chrono::duration_cast
#include <iomanip>    // Для std::put_time
#include <ctime>      // Для std::time_t, std::tm, localtime


#include "MemoryExchange/ClientMetaData.h"
#include "MemoryExchange/MetaSettings.h"

class TestClientPing{
public:
	TestClientPing(std::string name_memory): meta_setting_(name_memory), namenemory_(std::move(name_memory))
	{
//		meta_setting_ = MetaSettings(name_memory);
		client_meta_data_ = std::make_shared<ClientMetaData>(meta_setting_);
	}

	void run()
	{
    std::cout << "Enter ESC \n";
    int i = 0;
    while (true) {
      // Проверяем, нажата ли клавиша
      std::this_thread::sleep_for(std::chrono::seconds(1));

      if (_kbhit()) {
        int ch = _getch();
        if (ch == 27) {  // ESC ASCII код 27
          std::cout << "ESC enter ... i="<< i <<"\n";
          break;
        }
      }
/*
      // Выполняем задержку 0.5 секунды
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      std::cout << "  client  STATE MODE ==> " << AsKey(client_meta_data_->_mode)  << "\n";
      if(client_meta_data_->_mode == SateMode::Work 
					&& client_meta_data_->_transferWaiting == TransferWaiting::Transfer)
      {
        metadata_map ack = {
				{ AsKey(MdCommand::State), "clientCUDA" },
				{ "id_client", std::to_string(i)}
        };
        client_meta_data_->_transferWaiting = TransferWaiting::Waiting;
        client_meta_data_->WriteMetaMap(ack);
      }
*/
      // Можно добавить действия цикла здесь
//      std::cout << " client i = "<< i <<"\n";

      auto now = std::chrono::system_clock::now();
      std::time_t current_time_t = std::chrono::system_clock::to_time_t(now);

      std::tm local_tm;
      // Используем localtime_s: первый параметр - указатель на tm, второй - указатель на time_t
      if (errno_t err = localtime_s(&local_tm, &current_time_t)) {
        std::cerr << "Error getting local time" << '\n';
        continue;
      }
      std::stringstream ss;
      ss << "Tick: " << std::put_time(&local_tm, "%H:%M:%S") << "  &  count " << i;
      std::cout << ss.str() << '\n';
      i++;
    }
    client_meta_data_->work_dispose();

	}
private:
	MetaSettings meta_setting_;
	std::string namenemory_;
	std::shared_ptr<ClientMetaData> client_meta_data_;
};

//SateMode _mode;
//TransferWaiting _transferWaiting;