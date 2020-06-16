//
//  Filter.hpp
//  Audeus
//
//  Created by Tejas Manjunath on 10/21/19.
//  Copyright © 2019 Tejas Manjunath. All rights reserved.
//

#ifndef Filter_h
#define Filter_h

#include <stdio.h>
#include <math.h>
#include "Definitions.h"


using namespace std;

class CFilter
{
public:

	struct coeff
	{
		float b0, b1, b2, a0, a1, a2;
	};

	CFilter();
	coeff set_HS_coeffs();
	coeff set_HP_coeffs();
	void compute(float **input, float **output, coeff coeffs);
	
	coeff HS_Coefficients;
	coeff HP_Coefficients;
	float **output;

private:

	float pi;
	float *d0_z1;
	float *d1_z1;
	float *d0;
	float *d1;
};

#endif /* Filter_h */
