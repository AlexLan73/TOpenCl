#pragma once

#include <map>
#include <string>
#include <any> // если нужен аналог object из C#
#include "IInject.h"

using Params = std::map<std::string, std::any>;

struct IUnderTask : public IInject{
  virtual ~IUnderTask() = default;
  virtual int id() const = 0;
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void pause() = 0;
  virtual void set_params(const Params&) = 0;
  virtual Params get(int ind = -1) const = 0;
};
