#ifndef __WELL512_H__
#define __WELL512_H__
#include <ctime>
#include <random>

void INIT_WELL512(void);
unsigned long WELLRNG512(void);
unsigned long WELLRNG512_limit(int min, int max);

#endif