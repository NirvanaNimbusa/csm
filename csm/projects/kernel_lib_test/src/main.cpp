//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: main.cpp
// Author: Pavel Morozkin
// Date: August 18th 2013
// Revised: August 18th 2013
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
#include "kernel.h"

int main(int argc, char **argv)
{

	int galois_field_degree = 7;
	int bch_code_length = 127;
	int error_correction = 7;
	int decoded_seq_buf_size_frames = 2;
	double ber = 0.01;
	char* in_file = "test_files\\img_02.gif";
	char* out_file_postfix = "transferred";
	
	return kernel_run(BCH_MODE, USE_BER_CHANNEL, galois_field_degree, bch_code_length,
		error_correction, decoded_seq_buf_size_frames, ber, 0, 0, out_file_postfix, in_file, 0);
}