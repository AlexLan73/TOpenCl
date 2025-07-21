// OpenCltest.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#pragma execution_character_set("utf-8")

#include <iostream>
#include <memory>
#include "LibTest/TestClientPing.h"
#include "CudaModule/MCuda.h"
int main()
{
	auto _test_ping = std::make_shared<TestClientPing>("CUDA");
	_test_ping->run();


//	auto cuda_modules_ = std::make_unique<cuda_module::CudaModule>();

}

