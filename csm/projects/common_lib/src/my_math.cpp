//*******************************************************************************
// Title: Communication System Modeler v.1.0
// File: my_math.cpp
// Author: Pavel Morozkin
// Date: May 31th 2013
// Revised: May 31th 2013
//*******************************************************************************
// NOTE:
// The author is not responsible for any malfunctioning of this program, nor for
// any damage caused by it. Please include the original program along with these
// comments in any redistribution.
//
// For more information and suggestions, please email me: pavelmorozkin@yandex.ru
//
// COPYRIGHT NOTICE: This computer program is free for non-commercial purposes.
// You may implement this program for any non-commercial application. You may
// also implement this program for commercial purposes, provided that you
// obtain my written permission. Any modification of this program is covered 
// by this copyright.
//
// Copyright (c) 2013, Pavel Morozkin. All rights reserved.
//*******************************************************************************
#include "my_math.h"

// return a random number: uniform distribution, value in [0;1]
double uniform_distr ()
{
	return ((double)rand()/RAND_MAX);
}

// return a randon number: exponential distribution, value >= 0
long double exponential_distr (long double interarrival_rate)
{
	double U;

	do
	{
		U = uniform_distr();
	} while (U == 0);

	return (long double) (-1 / interarrival_rate ) * log (U);
}