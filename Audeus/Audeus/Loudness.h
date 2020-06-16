//
//  Loudness.hpp
//  Audeus
//
//  Created by Tejas Manjunath on 10/21/19.
//  Copyright © 2019 Tejas Manjunath. All rights reserved.
//

#ifndef Loudness_h
#define Loudness_h

#include <stdio.h>
#include <math.h>
#include "Filter.h"
#include "Definitions.h"

using namespace std;

class CLoudness
{
public:

	CLoudness();

	float *z;
	float *val;
	float *output;
    
	void compute_loudness(float **input, float *output);
	
	float **y_HS;
	float **y_HP;
    
private:
    
	float T_g;
};


#endif /* Loudness_h */
