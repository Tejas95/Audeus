#pragma once

#include <stdio.h>
#include <math.h>
#include <portaudio.h>
#include "Loudness.h"
#include "Gain.h"
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
	CLoudness Cloud;

	int LoudnessMode;
	float *LoudnessValues;

	int numBlocks;

	float **output;
	float **input;
	float **inputBufferForGain;
	float *gain1;
	float *gain;
	float *loud_I;
	float *loud_O;

	int BlocksizeTemp;
	int noiseLoudnessLevelTemp;
	int inputLoudnessLevelTemp;
	int gainLevelTemp;

	float **noiseLoudnessLevelsBuffer;
	float *noiseLoudnessLevelBufferAvg;
	float **inputLoudnessLevelsBuffer;
	float *inputLoudnessLevelBufferAvg;
	
	int numberOfNoiseLoudnessValues;
	int numberOfInputLoudnessValues;
	int numberOfGainValues;

	float *loudnessReferenceValues;
	float *loudnessCustomValues;

	string noiseLoudnessLevelStart;
	string noiseLoudnessLevelIsDone;
	string inputLoudnessLevelStart;
	string inputLoudnessLevelIsDone;
	string gainLevelStart;
	string gainLevelIsDone;
};