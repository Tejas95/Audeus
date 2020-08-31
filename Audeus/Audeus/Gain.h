//
//  Gain.hpp
//  Audeus
//
//  Created by Tejas Manjunath on 10/28/19.
//  Copyright © 2019 Tejas Manjunath. All rights reserved.
//

#ifndef Gain_h
#define Gain_h

#include <stdio.h>
#include <iostream>
#include "Definitions.h"
#include "Loudness.h"

using namespace std;


class CGain
{

public:

	CGain();

	void cal_gain(float *loud_in, float *loud_ref, float *gain);

private:
	CLoudness Loud;
	float *loudnessDifference;

};
#endif /* Gain_h */
