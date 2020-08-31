
#ifndef GainRamping_h
#define GainRamping_h

#include <stdio.h>
#include <iostream>
#include "Definitions.h"
#include "Loudness.h"

using namespace std;


class CGainRamping
{

public:

	float* increment;
	int count;
	CGainRamping();

	void cal_gainRamping(const float** in, float** out, float* startGain, float* endGain);

};
#endif /* GainRamping_h */
