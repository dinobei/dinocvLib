#ifndef __WELL512_H__
#define __WELL512_H__
#include <ctime>
#include <random>

//#define WELLRNG512_LIMIT(min, max) ((min) + ((WELLRNG512()) % ((max)-(min)+1)))

void INIT_WELL512(void);
unsigned long WELLRNG512(void);
unsigned long WELLRNG512_LIMIT(int min, int max);

#endif