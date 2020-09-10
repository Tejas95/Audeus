#pragma once

#define NOMINMAX

#include <stdio.h>
#include <math.h>
#include <portaudio.h>
#include <algorithm>
#include "Loudness.h"
#include "Gain.h"
#include "GainRamping.h"
#include "Definitions.h"
#include <iostream>
#include <string>
#include <dsound.h>
#include <dsconf.h>

using namespace std;

class CAudeus
{

public:
	CAudeus();
	void calculateAverage(float **input, float *output, int numberOfNoiseValues);

	CGain Cgain;
	CGainRamping CgainRamping;
	CLoudness Cloud;

	int LoudnessMode;
	float *LoudnessValues;

	int numBlocks;

	float **output;
	float **input;
	//float **inputBufferForGain;
	float *gain1;
	float *gain;
	float* adaptiveEndGain;
	float* increment;
	float *loud_I;
	float *loud_O;
	float* loud_output;

	int BlocksizeTemp;
	int noiseLoudnessLevelTemp;
	int inputLoudnessLevelTemp;
	int gainLevelTemp;

	float **noiseLoudnessLevelsBuffer;
	float *noiseLoudnessLevelBufferAvg;
	float *noiseLoudnessLevelBufferMax;
	float* noiseArray;
	float **inputLoudnessLevelsBuffer;
	float *inputLoudnessLevelBufferAvg;
	
	int numberOfNoiseLoudnessValues;
	int numberOfInputLoudnessValues;
	int numberOfGainValues;

	float *loudnessReferenceValues;
	float *loudnessValues;

	string noiseLoudnessLevelStart;
	string noiseLoudnessLevelIsDone;
	string inputLoudnessLevelStart;
	string inputLoudnessLevelIsDone;
	string gainLevelStart;
	string gainLevelIsDone;
	string adaptiveGainLevelStart;
	string adaptiveGainLevelIsDone;

	int channelTemp;
};