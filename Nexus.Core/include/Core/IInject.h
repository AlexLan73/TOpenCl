#pragma once
#include <memory>
#include "Logger/ILogger.h"
#include "DataContracts/IDataContext.h"

struct  IInject
{
  void inject(const std::shared_ptr<ILogger>& logger, const std::shared_ptr<IDataContext>& data_context) { // универсальный метод для DI
    logger_ = logger;;
    data_context_ = data_context;
  }
  std::shared_ptr<ILogger> logger_;
  std::shared_ptr<IDataContext> data_context_;

};

