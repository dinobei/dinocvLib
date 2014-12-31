#include "well512.h"

/* initialize state to random bits */
static unsigned long state[16];
/* init should also reset this to 0 */
static unsigned int index = 0;
/* return 32 bit random number */
void INIT_WELL512(void)
{
	srand((unsigned int)time(NULL));

	for(int i = 0 ; i < 15 ; i++)
	{
		state[i] = rand();
	}
}

unsigned long WELLRNG512(void)
{
	unsigned long a, b, c, d;
	a = state[index];
	c = state[(index+13)&15];
	b = a^c^(a<<16)^(c<<15);
	c = state[(index+9)&15];
	c ^= (c>>11);
	a = state[index] = b^c;
	d = a^((a<<5)&0xDA442D20UL);
	index = (index + 15)&15;
	a = state[index];
	state[index] = a^b^d^(a<<2)^(b<<18)^(c<<28);
	return state[index];
}

// min <= result <= max
unsigned long WELLRNG512_limit(int min, int max)
{
	unsigned long a, b, c, d;
	a = state[index];
	c = state[(index+13)&15];
	b = a^c^(a<<16)^(c<<15);
	c = state[(index+9)&15];
	c ^= (c>>11);
	a = state[index] = b^c;
	d = a^((a<<5)&0xDA442D20UL);
	index = (index + 15)&15;
	a = state[index];
	state[index] = a^b^d^(a<<2)^(b<<18)^(c<<28);
	return min + (state[index] % (max-min+1));
}