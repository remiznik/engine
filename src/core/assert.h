#pragma once


#define ASSERT(exp) \
	if (!(exp))		\
	{				\
		throw;		\
	}
