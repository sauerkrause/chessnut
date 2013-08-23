#ifndef __RANDOM_H_INCLUDED__
#define __RANDOM_H_INCLUDED__

#include "tutils\dsfmt.h"

class Random
{
public:
	Random(unsigned long seed = 1234);
	~Random();
	void Init(unsigned long seed);
	void Release();
	unsigned long GenerateInt();
	double GenerateDouble();
private:

	tutils::dsfmt19937 rand;
};

#endif