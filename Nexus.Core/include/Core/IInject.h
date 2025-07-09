#pragma once
#include <memory>
#include "Logger/ILogger.h"
#include "DataContracts/IDataContext.h"

struct  IInject
{
  void inject(const auto& injector) { // универсальный метод для DI
    // Получаем зависимости из инжектора
    logger_ = injector.template create<std::shared_ptr<ILogger>>();
    data_context_ = injector.template create<std::shared_ptr<IDataContext>>();
    // Можно получить и другие зависимости

  }
  std::shared_ptr<ILogger> logger_;
  std::shared_ptr<IDataContext> data_context_;

};

