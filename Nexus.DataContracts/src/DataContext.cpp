#include "pch.h"
#include "DataContracts/DataContext.h"
#include "interfaces/i_memory_config_channel.h"
#include <iostream>


DataContext::DataContext()
{
	std::cerr << "  Start DataContext " << '\n';
  metadata_map_base_[name_type_] = "-9999";
  initialization_channels();
  //	std::map<std::string, std::string> m_base;
  //  m_base["type"] = "-999";
	//const std::string& name, server_client role, IMemoryDataHandler* handler
//	auto memory_ = std::make_unique<MemoryProcessor>();

}
/*
void DataContext::send(std::string s)
{
//	IDataContext::send(s);
	std::cerr << "  DataContext -->>  " << s << '\n';

}
*/
void DataContext::send_channel(std::vector<ILoggerChannel>& data, metadata_map& meta)
{
  send(0, data, meta);
}

void DataContext:: send_channel(std::vector<IIdValueDtChannel>& data, metadata_map& meta)
{
  send(1, data, meta);
}
void  DataContext::send_channel(std::vector<IIdVecValueDtChannel>& data, metadata_map& meta)
{
  send(2, data, meta);
}


void DataContext::send_logger(ILoggerChannel msg)
{
	std::cerr << "  FROM LOGGER  ->  "<< msg.log << '\n';
}



void DataContext::initialization_channels()
{
  // Регистрация каналов (например, для температур и логов)

  for (uint32_t i = e_logger; i < COUNT_TYPE_DATA_CHANNEL; ++i) {
    data_type_ids dt = static_cast<data_type_ids>(i);

    switch (dt) {
    case e_logger:
      register_channel<ILoggerChannel>(0);     // Логи
      std::cout << "e_logger\n";
      break;
    case e_id_value_dt_channel:
      register_channel<IIdValueDtChannel>(1);     // Логи
      std::cout << "e_id_value_dt_channel \n";
      break;
    case e_id_vec_value_dt_channel:
      register_channel<IIdVecValueDtChannel>(2);     // Логи
      std::cout << "e_id_vec_value_dt_channel \n";
      break;

    default:
      std::cout << "Unknown value\n";
      break;
    }
  }

}


