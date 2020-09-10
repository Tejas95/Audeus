#include "stdafx.h"
#include "GainRamping.h"
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <iostream>

using namespace std;

CGainRamping::CGainRamping()
{
	increment = new float[NUM_CHANNELS];
}

void CGainRamping::cal_gainRamping(const float** in, float** out, float* startGain, float* endGain)
{
	for (int ch = 0; ch < NUM_CHANNELS; ch++)
	{
		//startGain[ch] = pow(10, startGain[ch] / 20);
		endGain[ch] = pow(10, endGain[ch] / 20);
		
		increment[ch] = (endGain[ch] - startGain[ch]) / (FRAMES_PER_BUFFER);

		for (int sample = 0; sample < FRAMES_PER_BUFFER; sample++)
		{
			out[ch][sample] = in[ch][sample] * startGain[ch];
			startGain[ch] += increment[ch];
		}
	}
}