#pragma once
#include <memory>
#include "Logger/ILogger.h"
#include "DataContracts/IDataContext.h"
//class ILogger;
//class IDataContext;

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

//
//
//class ModuleUnderTask1 : public IUnderTask {
//public:
//  int id() const override { return 1; }
//
//  void inject(const auto& injector) override {
//    // Получаем зависимости из инжектора
//    logger_ = injector.template create<std::shared_ptr<ILogger>>();
//    data_context_ = injector.template create<std::shared_ptr<IDataContext>>();
//    // Можно получить и другие зависимости
//  }
//
//  void do_work() {
//    if (logger_) {
//      logger_->log("ModuleUnderTask1 is working");
//    }
//  }
//
//private:
//  std::shared_ptr<ILogger> logger_;
//  std::shared_ptr<IDataContext> data_context_;
//};
