//
//  Filter.cpp
//  Audeus
//
//  Created by Tejas Manjunath on 10/21/19.
//  Copyright © 2019 Tejas Manjunath. All rights reserved.
//

#include "stdafx.h"
#include "Filter.h"
#include <iostream>

using namespace std;


CFilter::CFilter()
{	
	//Filter State Variables
	d0_z1 = new float[NUM_CHANNELS];
	d1_z1 = new float[NUM_CHANNELS];
	d0 = new float[NUM_CHANNELS];
	d1 = new float[NUM_CHANNELS];
	
	memset(d0_z1, 0, sizeof(float) * NUM_CHANNELS);
	memset(d1_z1, 0, sizeof(float) * NUM_CHANNELS);
	memset(d0, 0, sizeof(float) * NUM_CHANNELS);
	memset(d1, 0, sizeof(float) * NUM_CHANNELS);


	output = new float *[NUM_CHANNELS];
	for (int i = 0; i < NUM_CHANNELS; i++)
		output[i] = new float[BLOCK_SIZE];

	HS_Coefficients = set_HS_coeffs();
	HP_Coefficients = set_HP_coeffs();
	pi = 3.14159265359;
}

CFilter::coeff CFilter::set_HS_coeffs()
{
	//High Shelf Coefficients
	float G = 4.0;
	float Q = pow(2, -0.5);
	float fc = 1500.0;
	float A = pow(10, (G / 40.0));
	float w0 = 2.0 * pi * (fc / SAMPLE_RATE);
	float alpha = sin(w0) / (2.0 * Q);

	float b0 = A * ((A + 1) + (A - 1) * cos(w0) + 2 * pow(A, 0.5) * alpha);
	float b1 = -2 * A * ((A - 1) + (A + 1) * cos(w0));
	float b2 = A * ((A + 1) + (A - 1) * cos(w0) - 2 * pow(A, 0.5) * alpha);
	float a0 = (A + 1) - (A - 1) * cos(w0) + 2 * pow(A, 0.5) * alpha;
	float a1 = 2 * ((A - 1) - (A + 1) * cos(w0));
	float a2 = (A + 1) - (A - 1) * cos(w0) - 2 * pow(A, 0.5) * alpha;
	coeff coefficients = { b0 / a0, b1 / a0, b2 / a0, a0 / a0, a1 / a0, a2 / a0 };

	return coefficients;
}

CFilter::coeff CFilter::set_HP_coeffs()
{
	//High Pass Coefficients
	float Q = 0.5;
	float fc = 38.0;
	float w0 = 2.0 * pi * (fc / SAMPLE_RATE);
	float alpha = sin(w0) / (2.0 * Q);

	float b0 = (1 + cos(w0)) / 2;
	float b1 = -(1 + cos(w0));
	float b2 = (1 + cos(w0)) / 2;
	float a0 = 1 + alpha;
	float a1 = -2 * cos(w0);
	float a2 = 1 - alpha;
	coeff coefficients = { b0 / a0, b1 / a0, b2 / a0, a0 / a0, a1 / a0, a2 / a0 };
	
	return coefficients;
}


void CFilter::compute(float **input, float **output, coeff coeffs)
{   
    for (int i = 0; i < NUM_CHANNELS; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            output[i][j]    = coeffs.b0 * input[i][j]                            + d0_z1[i];
            d0[i]           = coeffs.b1 * input[i][j] - coeffs.a1 * output[i][j] + d1_z1[i];
            d1[i]           = coeffs.b2 * input[i][j] - coeffs.a2 * output[i][j];
            
            d0_z1[i] = d0[i];
            d1_z1[i] = d1[i];
        }
    }
}
