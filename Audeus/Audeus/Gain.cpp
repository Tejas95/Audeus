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


void CGain::cal_gain(float **audio_in, float *loud_in, float *loud_ref, float *gain)
{
	while (true)
	{
		//Gain Calculation
		for (int i = 0; i < NUM_CHANNELS; i++)
		{
			loudnessDifference[i] = loud_in[i] - loud_ref[i];
			if (loudnessDifference[i] > -3 && loudnessDifference[i] < 3)
			{
				cout << "Gain Level is: " << gain[i] << endl;
				cout << "Loudness Level is: " << loud_in[i] << endl;
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
	}
}
