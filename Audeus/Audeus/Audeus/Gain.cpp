//
//  Gain.cpp
//  Audeus
//
//  Created by Tejas Manjunath on 10/28/19.
//  Copyright © 2019 Tejas Manjunath. All rights reserved.
//

#include "stdafx.h"
#include "Gain.h"
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <iostream>

using namespace std;

CGain::CGain()
{
	loudnessDifference = new float[NUM_CHANNELS];
}


void CGain::cal_gain(float *loud_in, float *loud_ref, float *gain)
{
	for (int i = 0; i < NUM_CHANNELS; i++)
	{
		gain[i] = loud_ref[i] - loud_in[i];
		gain[i] = pow(10, gain[i] / 20);
		cout << "Gain Level for channel " << i << " is: " << gain[i] << endl;
	}

	/*
	while (true)
	{
		//Gain Calculation
		for (int i = 0; i < NUM_CHANNELS; i++)
		{
			loudnessDifference[i] = loud_in[i] - loud_ref[i];


			if (loudnessDifference[i] > -3 && loudnessDifference[i] < 3)
			{
				cout << "Gain Level for channel 1 is: " << gain[0] << endl;
				cout << "Gain Level for channel 2 is: " << gain[1] << endl;
				return;
			}

			else
			{
				//IF GAIN NEEDS TO BE ADDED TO INPUT
				if (loudnessDifference[i] < -30)
					gain[i] += 1;
				else if (loudnessDifference[i] < -10 && loudnessDifference[i] > -30)
					gain[i] += 0.5;
				else if (loudnessDifference[i] < -5 && loudnessDifference[i] > -10)
					gain[i] += 0.25;
				else if (loudnessDifference[i] < -3 && loudnessDifference[i] > -5)
					gain[i] += 0.1;
				

				//IF GAIN NEEDS TO BE SUBTRACTED FROM INPUT
				if (loudnessDifference[i] > 20)
					gain[i] -= 0.5;
				else if (loudnessDifference[i] > 10 && loudnessDifference[i] < 20)
					gain[i] -= 0.25;
				else if (loudnessDifference[i] > 5 && loudnessDifference[i] < 10)
					gain[i] -= 0.1;
				else if (loudnessDifference[i] > -3 && loudnessDifference[i] < 5)
					gain[i] -= 0.05;
				
				for (int j = 0; j < BLOCK_SIZE; j++)
					audio_in[i][j] = gain[i] * audio_in[i][j];
				
				Loud.compute_loudness(audio_in,loud_in);
			}
		}
	}*/
}
