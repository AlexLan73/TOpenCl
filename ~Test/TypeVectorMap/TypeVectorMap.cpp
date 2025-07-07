// TypeVectorMap.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <variant>
#include <vector>

#include <iostream>
#include <memory>
#include <map>
#include <typeindex>
#include <functional>

struct IMyRxCPP {
  virtual ~IMyRxCPP() = default;
  virtual void info() = 0;
  int id;
};

struct  struct1:public IMyRxCPP
{
  struct1(){
    id = 1;
  }
	
};
struct  struct2 :public IMyRxCPP
{
  struct2() {
    id = 2;
  }
};
struct  struct3 :public IMyRxCPP
{
  struct3() {
    id = 3;
  }
};
struct  struct4 :public IMyRxCPP
{
  struct4() {
    id = 4;
  }

};


template<typename T>
struct MyRxCPP : IMyRxCPP {
  void info() override {
    std::cout << "MyRxCPP for " << typeid(T).name() << "\n";
  }
};

std::map<std::type_index, std::shared_ptr<IMyRxCPP>> m_type;

template<typename T>
void registerType() {
  m_type[typeid(T)] = std::make_shared<MyRxCPP<T>>();
}



int main() {
  registerType<struct1>();
  registerType<struct2>();
  registerType<struct3>();
  registerType<struct4>();


  for (auto& [type, obj] : m_type) {
    std::cout << "Stored type: " << type.name() << "\n";
    std::cout << " id= " << obj->id << "\n";
  }

  return 0;
}