// OpenCltest.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#pragma execution_character_set("utf-8")

#include <iostream>
#include <memory>

#include "CudaModule/MCuda.h"
int main()
{
	auto cuda_modules_ = std::make_unique<cuda_module::CudaModule>();

}

