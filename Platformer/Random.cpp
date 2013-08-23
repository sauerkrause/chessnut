#include "Random.h"
#include <limits.h>

Random::Random(unsigned long seed)
{
	Init(seed);
}

Random::~Random()
{
	Release();
}

void Random::Init(unsigned long seed)
{
	rand = tutils::dsfmt19937::dsfmt(seed);

}

void Random::Release()
{
	return;
}

unsigned long Random::GenerateInt()
{
	unsigned long retVal;
	retVal = (unsigned long)(ULONG_MAX * rand.gen());
	return retVal;
}
double Random::GenerateDouble()
{
	return rand.gen();
}