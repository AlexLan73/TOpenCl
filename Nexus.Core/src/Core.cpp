#include "pch.h"
#include "Core/Core.h"

#include <iostream>

Core::Core()
{
	std::cerr << "  Start Core " << '\n';
	auto _data_context = std::make_shared<DataContext>();
}
