//
//  Loudness.cpp
//  Audeus
//
//  Created by Tejas Manjunath on 10/21/19.
//  Copyright © 2019 Tejas Manjunath. All rights reserved.
//

#include "stdafx.h"
#include "Loudness.h"
#include <iostream>


using namespace std;

CLoudness::CLoudness()
{	
	T_g = 0.400;

	output = new float[NUM_CHANNELS];
	z = new float[NUM_CHANNELS];
	val = new float[NUM_CHANNELS];
	val[0] = 0;

	y_HS = new float *[NUM_CHANNELS];
	y_HP = new float *[NUM_CHANNELS];

	for (int i = 0; i < NUM_CHANNELS; i++)
	{
		y_HS[i] = new float[BLOCK_SIZE];
		y_HP[i] = new float[BLOCK_SIZE];
	}
}


void CLoudness::compute_loudness(float **input, float *output)
{
	CFilter Filt;

	Filt.compute(input, y_HS, Filt.HS_Coefficients);		//High Shelf Filter
	Filt.compute(y_HS, y_HP, Filt.HP_Coefficients);           //High Pass Filter

	//Loudness Calculation
    for (int i = 0; i < NUM_CHANNELS; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            val[i] = val[i] + (y_HP[i][j] * y_HP[i][j]);
        }
        
        z[i] =  pow(T_g * SAMPLE_RATE, -1) * val[i];

        output[i] = -0.691 + 10.0 * log10(z[i]);
        val[i] = 0;
    }
    
}
