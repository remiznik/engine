#pragma once

#include <iostream>

#define ASSERT(exp, msg)	\
	if (!(exp))				\
	{						\
		std::cout << msg << std::endl; \
		throw;				\
	}
