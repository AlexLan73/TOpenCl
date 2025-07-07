#include "pch.h"
#include "DataContracts/DataContext.h"
#include "interfaces/i_memory_config_channel.h"
#include <iostream>


DataContext::DataContext()
{
	std::cerr << "  Start DataContext " << '\n';
  metadata_map_base_[name_type_] = "-9999";
  //	std::map<std::string, std::string> m_base;
  //  m_base["type"] = "-999";
	//const std::string& name, server_client role, IMemoryDataHandler* handler
//	auto memory_ = std::make_unique<MemoryProcessor>();

}

void DataContext::send(std::string s)
{
//	IDataContext::send(s);
	std::cerr << "  DataContext -->>  " << s << '\n';

}

void DataContext::send(const IVectorChannel vector_channel)
{
	IDataContext::send(vector_channel);
}

void DataContext::send(const IValueChannel value_channel)
{
	IDataContext::send(value_channel);
}

void DataContext::addTask(std::function<void()> task)
{
}

void DataContext::send_logger(ILoggerChannel msg)
{
	std::cerr << "  FROM LOGGER  ->  "<< msg.log << '\n';
}

void DataContext::send_channel(std::vector<ILoggerChannel>& data)
{
  send(0, data);
}

void DataContext::send_channel(std::vector<IDateTimeVariableChannel>& data)
{
  send(1, data);
}

void DataContext::send_channel(std::vector<IVectorChannel>& data)
{
  send(2, data);
}

void DataContext::send_channel(std::vector<IMatrixChannel>& data)
{
  send(3, data);
}

void DataContext::send_channel(std::vector<IRecResultChannel>& data)
{
  send(4, data);
}

void DataContext::send_channel(std::vector<IValueChannel>& data)
{
  send(5, data);
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
    case e_date_time_variable:
      register_channel<IDateTimeVariableChannel>(1);     // Логи
      std::cout << "e_date_time_variable\n";
      break;
    case e_vector:
      register_channel<IVectorChannel>(2);     // Логи
      std::cout << "e_vector\n";
      break;
    case e_matrix:
      register_channel<IMatrixChannel>(3);     // Логи
      std::cout << "e_matrix\n";
      break;
    case e_rec_result:
      register_channel<IRecResultChannel>(4);     // Логи
      std::cout << "e_rec_result\n";
      break;
    case e_value:
      register_channel<IValueChannel>(5);     // Логи
      std::cout << "e_value\n";
      break;
    //case e_dt_record:
    //  std::cout << "e_dt_record\n";
    //  break;

    default:
      std::cout << "Unknown value\n";
      break;
    }
  }

}








/*

 
 */
