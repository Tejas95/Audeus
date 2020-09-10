// Audeus.cpp : Defines the entry point for the console application.
//


/** @file main.cpp
@ingroup Audeus
@brief Estimate Loudness and adjust gain
@author Tejas Manjunath
*/
/*
* $Id$
*
* This program uses the PortAudio Portable Audio Library.
* For more information see: http://www.portaudio.com
* Copyright (c) 1999-2000 Ross Bencina and Phil Burk
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files
* (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
* ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
* CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
* The text above constitutes the entire PortAudio license; however,
* the PortAudio community also makes the following non-binding requests:
*cd D
* Any person wishing to distribute modifications to the Software is
* requested to send the modifications to the original developer so that
* they can be incorporated into the canonical version. It is also
* requested that these non-binding requests be included along with the
* license above.
*/

#include "stdafx.h"
#include "Audeus.h"


CAudeus::CAudeus()
{
	numBlocks = BLOCK_SIZE / FRAMES_PER_BUFFER;

	BlocksizeTemp = 0;
	noiseLoudnessLevelTemp = 0;
	inputLoudnessLevelTemp = 0;
	gainLevelTemp = 0;
	channelTemp = 0;
	
	output = new float *[NUM_CHANNELS]; //num_channels x blocksize
	input = new float *[NUM_CHANNELS];
	//inputBufferForGain = new float*[NUM_CHANNELS];

	gain = new float[NUM_CHANNELS];
	adaptiveEndGain = new float[NUM_CHANNELS];
	increment = new float[NUM_CHANNELS];

	loud_I = new float[NUM_CHANNELS];
	loud_O = new float[NUM_CHANNELS];
	memset(loud_I, 0, sizeof(float) * NUM_CHANNELS);
	memset(loud_O, 0, sizeof(float) * NUM_CHANNELS);

	noiseLoudnessLevelsBuffer = new float*[NUM_CHANNELS];
	noiseLoudnessLevelBufferAvg = new float[NUM_CHANNELS];
	noiseLoudnessLevelBufferMax = new float[NUM_CHANNELS];
	noiseArray = new float[numberOfNoiseLoudnessValues];
	inputLoudnessLevelsBuffer = new float*[NUM_CHANNELS];
	inputLoudnessLevelBufferAvg = new float[NUM_CHANNELS];
	memset(noiseLoudnessLevelBufferAvg, 0, sizeof(float) * NUM_CHANNELS);
	memset(noiseLoudnessLevelBufferMax, 0, sizeof(float) * NUM_CHANNELS);
	memset(inputLoudnessLevelBufferAvg, 0, sizeof(float) * NUM_CHANNELS);
	memset(increment, 0, sizeof(float) * NUM_CHANNELS);

	numberOfNoiseLoudnessValues = 10;
	numberOfInputLoudnessValues = 10;
	numberOfGainValues = 10;

	noiseLoudnessLevelStart = "YES";
	noiseLoudnessLevelIsDone = "NO";
	inputLoudnessLevelStart = "NO";
	inputLoudnessLevelIsDone = "NO";
	gainLevelStart = "NO";
	gainLevelIsDone = "NO";
	adaptiveGainLevelStart = "YES";
	adaptiveGainLevelIsDone = "YES";

	loudnessReferenceValues = new float[NUM_CHANNELS];
	loudnessValues = new float[NUM_CHANNELS];
	

	for (int i = 0; i < NUM_CHANNELS; i++)
	{
		output[i] = new float[BLOCK_SIZE];
		input[i] = new float[BLOCK_SIZE];
		//inputBufferForGain[i] = new float[BLOCK_SIZE];
		noiseLoudnessLevelsBuffer[i] = new float[numberOfNoiseLoudnessValues];
		inputLoudnessLevelsBuffer[i] = new float[numberOfInputLoudnessValues];
		memset(output[i], 0, sizeof(float) * BLOCK_SIZE);
		memset(input[i], 0, sizeof(float) * BLOCK_SIZE);
		memset(noiseLoudnessLevelsBuffer[i], 0, sizeof(float) * numberOfNoiseLoudnessValues);
		memset(inputLoudnessLevelsBuffer[i], 0, sizeof(float) * numberOfInputLoudnessValues);

		loudnessReferenceValues[i] = -30;
		gain[i] = 1;
	}
}

void CAudeus::calculateAverage(float **input, float *output, int numberOfNoiseValues)
{
	for (int i = 0; i < NUM_CHANNELS; i++)
	{
		for (int j = 0; j < numberOfNoiseValues; j++)
		{
			output[i] += input[i][j];
		}
	
		output[i] = output[i] / numberOfNoiseValues;
	}
}

static int AudioCallback(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData);

static int AudioCallback(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	float **out = (float**)outputBuffer;
	const float **in = (const float**)inputBuffer;

	unsigned int i;
	unsigned int j;
	(void)timeInfo; /* Prevent unused variable warnings. */
	(void)statusFlags;

	CAudeus* AUD = (CAudeus*)userData;
	

	for (i = 0; i < NUM_CHANNELS; i++)
	{
		if (inputBuffer == NULL)
		{
			for (j = 0; j < framesPerBuffer; j++)
			{
				out[i][j] = 0;  // silent
			}
		}

		else
		{
			for (j = 0; j < framesPerBuffer; j++)
			{
				AUD->input[i][(j + (framesPerBuffer * AUD->BlocksizeTemp))] = in[i][j];
				AUD->output[i][(j + (framesPerBuffer * AUD->BlocksizeTemp))] = AUD->gain[i] * in[i][j];

				if (AUD->gainLevelIsDone.compare("YES") == 0)
				{	
					if (AUD->BlocksizeTemp == AUD->numBlocks)
					{
						AUD->Cloud.compute_loudness(AUD->output, AUD->loud_O);

						if (AUD->loud_O[i] > AUD->loudnessReferenceValues[i] + 25)
						{
							AUD->adaptiveEndGain[i] = pow(10, (AUD->loudnessReferenceValues[i] - AUD->loud_O[i]) / 20);
							AUD->increment[i] = (AUD->adaptiveEndGain[i] - AUD->gain[i]) / (FRAMES_PER_BUFFER);

							AUD->adaptiveGainLevelIsDone = "NO";
						}
					}
				}

				if (AUD->adaptiveGainLevelIsDone.compare("NO") == 0)

					if (AUD->gain[i] != AUD->adaptiveEndGain[i])
						AUD->gain[i] = AUD->gain[i] - AUD->increment[i];
					else
						AUD->adaptiveGainLevelIsDone = "YES";

				out[i][j] = AUD->gain[i] * in[i][j];
			}
		}
	}

	AUD->BlocksizeTemp = AUD->BlocksizeTemp + 1;


	/***************************************** THIS PART IS THE STEPS ***********************************************/


	//Check if number of samples required to calculate loudness is reached
	if (AUD->BlocksizeTemp == AUD->numBlocks)
	{
		/* START WITH NOISE */
		
		if (AUD->noiseLoudnessLevelStart.compare("YES") == 0)
		{
			if (AUD->noiseLoudnessLevelTemp == 0 && AUD->channelTemp == 0)
				printf("Noise loudness level detection has started \n");

			//Creates noise level buffer
			if (AUD->noiseLoudnessLevelIsDone.compare("NO") == 0)
			{
				//Noise detection with cross talk 
				if (AUD->noiseLoudnessLevelTemp == AUD->numberOfNoiseLoudnessValues - 1)
				{
					cout << "Noise level detection for Channel " << AUD->channelTemp + 1 << " is complete" << endl;
					AUD->channelTemp += 1;
					AUD->noiseLoudnessLevelTemp = 0;
				}

				if (AUD->noiseLoudnessLevelTemp == 0)
				{
					cout << "Every channel except for channel " << AUD->channelTemp + 1 << " should play something" << endl;
				}

				//Calculates input loudness
				AUD->Cloud.compute_loudness(AUD->input, AUD->loud_I);

				//Store each channel loudness in temp buffer -> 10 values
				//for (int channel = 0; channel < NUM_CHANNELS; channel++)
				AUD->noiseLoudnessLevelsBuffer[AUD->channelTemp][AUD->noiseLoudnessLevelTemp] = AUD->loud_I[AUD->channelTemp];

				AUD->noiseLoudnessLevelTemp += 1;
				

				//Calculate average of 10 values and move on 
				if (AUD->noiseLoudnessLevelTemp == AUD->numberOfNoiseLoudnessValues -1 && AUD->channelTemp == NUM_CHANNELS - 1)
				{
					AUD->calculateAverage(AUD->noiseLoudnessLevelsBuffer, AUD->noiseLoudnessLevelBufferAvg, AUD->numberOfNoiseLoudnessValues);
					//for (int i = 0; i < NUM_CHANNELS; i++)
					//{
					//	memcpy(AUD->noiseArray, AUD->noiseLoudnessLevelsBuffer[i], sizeof(float) * AUD->numberOfNoiseLoudnessValues);
					//	AUD->noiseLoudnessLevelBufferMax[i] = max_element(AUD->noiseArray[i], AUD->noiseArray[i]);
					//}
					printf("Noise level detection is complete \n");
					cout << "Noise Level for channel 1 is: " << AUD->noiseLoudnessLevelBufferAvg[0] << endl;
					AUD->noiseLoudnessLevelIsDone = "YES";
					AUD->inputLoudnessLevelStart = "YES";
					printf("Input loudness level detection has started. Please play or sing anything for about 10 seconds \n");
				}
			}
		}


		/* CALCULATE INPUT LOUDNESS */

		if (AUD->inputLoudnessLevelStart.compare("YES") == 0)
		{
			if (AUD->inputLoudnessLevelIsDone.compare("NO") == 0)
			{
				//Calculates input loudness
				AUD->Cloud.compute_loudness(AUD->input, AUD->loud_I);

				//Store each channel loudness and input in temp buffer -> 10 values
				for (int channel = 0; channel < NUM_CHANNELS; channel++)
				{
					if (AUD->loud_I[channel] > AUD->noiseLoudnessLevelBufferAvg[channel])
					{
						AUD->inputLoudnessLevelsBuffer[channel][AUD->inputLoudnessLevelTemp] = AUD->loud_I[channel];
						AUD->inputLoudnessLevelTemp += 1;
						//memcpy(AUD->inputBufferForGain[channel], AUD->input[channel], sizeof(float)*BLOCK_SIZE);
					}
				}

				//Calculate average of 10 values and move on 
				if (AUD->inputLoudnessLevelTemp == AUD->numberOfInputLoudnessValues)
				{
					AUD->calculateAverage(AUD->inputLoudnessLevelsBuffer, AUD->inputLoudnessLevelBufferAvg, AUD->numberOfInputLoudnessValues);

					printf("Input level detection is complete \n");
					cout << "Input Level for channel 1 is: " << AUD->inputLoudnessLevelBufferAvg[0] << endl;
					cout << "Input Level for channel 2 is: " << AUD->inputLoudnessLevelBufferAvg[1] << endl;
					AUD->inputLoudnessLevelIsDone = "YES";
					AUD->gainLevelStart = "YES";
				}
			}
		}

		/* CALCULATE GAIN */

		if (AUD->gainLevelStart.compare("YES") == 0)
		{
			memcpy(AUD->loudnessValues, AUD->loudnessReferenceValues, sizeof(float) * NUM_CHANNELS);
			/*
			printf("Please choose a loudness mode. Select '1' to set all channels to the same loudness and select '2' to manually set the loudness values as desired \n");

			
			cin >> AUD->LoudnessMode;

			switch (AUD->LoudnessMode)
			{
			case(1):
				memcpy(AUD->LoudnessValues, AUD->loudnessReferenceValues, sizeof(float) * NUM_CHANNELS);
				break;
			case(2):
				for (int channel = 0; channel < NUM_CHANNELS; channel++)
				{
					cout << "Enter loudness value for Channel " << channel << endl;
					cin >> AUD->LoudnessValues[channel];
				}
				break;
			default:
				memcpy(AUD->LoudnessValues, AUD->loudnessReferenceValues, sizeof(float) * NUM_CHANNELS);
				break;
			}
			*/
		
			if (AUD->gainLevelIsDone.compare("NO") == 0)
			{
				AUD->Cgain.cal_gain(AUD->inputLoudnessLevelBufferAvg, AUD->loudnessValues, AUD->gain);
				printf("Gain is complete \n");
				AUD->gainLevelIsDone = "YES";
			}
		}


		//This section will now do adaptive gain changes. If loudness of a channel exceeds the loudness level set for that channel, then gradually
		//bring it back to the set level
		/*
		for (int ch = 0; ch < NUM_CHANNELS; ch++)
		{
		 	AUD->Cloud.compute_loudness(AUD->output, AUD->loud_I);

		 	if (AUD->loud_I[ch] > AUD->loudnessReferenceValues[ch] + 15)
		 	{
				
		 		AUD->adaptiveEndGain[ch] = AUD->loudnessReferenceValues[ch] - AUD->loud_I[ch];

		 		//Apply Gain Ramping
		 		AUD->CgainRamping.cal_gainRamping(in, out, AUD->gain, AUD->adaptiveEndGain);
					
		 		cout << AUD->gain[ch] << endl;
		 	}
		}
		*/
		AUD->BlocksizeTemp = 0;
	}
	
	return paContinue;
}

/*******************************************************************************************************************************************************************************/

int main(void);
int main(void)
{
	PaStreamParameters inputParameters, outputParameters;
	PaStream *stream;
	PaError err;

	CAudeus AUD;
	
	char c;

	err = Pa_Initialize();
	if (err != paNoError) goto error;

	inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
	if (inputParameters.device == paNoDevice) {
		fprintf(stderr, "Error: No default input device.\n");
		goto error;
	}

	inputParameters.channelCount = NUM_CHANNELS;       
	inputParameters.sampleFormat = PA_SAMPLE_TYPE | paNonInterleaved;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;

	outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
	if (outputParameters.device == paNoDevice) {
		fprintf(stderr, "Error: No default output device.\n");
		goto error;
	}
	outputParameters.channelCount = NUM_CHANNELS;       
	outputParameters.sampleFormat = PA_SAMPLE_TYPE | paNonInterleaved;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;


	err = Pa_OpenStream(
		&stream,
		&inputParameters,
		&outputParameters,
		SAMPLE_RATE,
		FRAMES_PER_BUFFER,
		NULL, /* paClipOff, */  /* we won't output out of range samples so don't bother clipping them */
		AudioCallback,
		(void*)&AUD);
	if (err != paNoError) goto error;

			
	err = Pa_StartStream(stream);
	if (err != paNoError) goto error;

	printf("Hit ENTER to stop program.\n");
	getchar();
	//cin >> c;
	//if (c == 'q')
//	{
	err = Pa_CloseStream(stream);
	if (err != paNoError) goto error;

	printf("Finished.");
	Pa_Terminate();
	return 0;
//	}

error:
	Pa_Terminate();
	fprintf(stderr, "An error occured while using the portaudio stream\n");
	fprintf(stderr, "Error number: %d\n", err);
	fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
	return -1;
}
