#ifndef __WELL512_H__
#define __WELL512_H__
#include "../common/def_os_selector_header.h"
#include <ctime>
#include <random>

//#define WELLRNG512_LIMIT(min, max) ((min) + ((WELLRNG512()) % ((max)-(min)+1)))

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void INIT_WELL512(void);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	unsigned long WELLRNG512(void);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	unsigned long WELLRNG512_LIMIT(int min, int max);

#endif