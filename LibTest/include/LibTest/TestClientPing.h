#pragma once
#include <conio.h>
#include <memory>
#include <string>

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
      if (_kbhit()) {
        int ch = _getch();
        if (ch == 27) {  // ESC ASCII код 27
          std::cout << "ESC enter ... i="<< i <<"\n";
          break;
        }
      }
      // Выполняем задержку 0.5 секунды
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));

      // Можно добавить действия цикла здесь
//      std::cout << " client i = "<< i <<"\n";
      i++;
    }
    client_meta_data_->work_dispose();

	}
private:
	MetaSettings meta_setting_;
	std::string namenemory_;
	std::shared_ptr<ClientMetaData> client_meta_data_;
};

