// OpenCltest.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#pragma execution_character_set("utf-8")
#include "interfaces/PulsedTimer.h"

#include <iostream>
#include <memory>
#include "LibTest/TestClientPing.h"
#include "CudaModule/MCuda.h"
int main()
{
	//PulseTimer::Instance().Subscribe(&mod);
	//PulseTimer::Instance().Start();
	//// ...
	//PulseTimer::Instance().Unsubscribe(&mod);
	//PulseTimer::Instance().Stop();

	PulseTimer::Instance().Start();
	auto _test_ping = std::make_shared<TestClientPing>("CUDA");
	_test_ping->run();
	PulseTimer::Instance().Start();

//	auto cuda_modules_ = std::make_unique<cuda_module::CudaModule>();

}

