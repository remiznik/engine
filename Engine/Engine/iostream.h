#ifndef _IOSTREAM_H
#define _IOSTREAM_H

#include <iostream>

#ifdef UNICODE
	#define tcout wcout
	#define tcin wcin
#else
	#define tcout cout
	#defint tcin cin
#endif 

#endif
