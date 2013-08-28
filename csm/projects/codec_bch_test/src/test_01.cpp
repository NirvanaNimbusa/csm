//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: test_01.cpp
// Author: Pavel Morozkin
// Date: August 28th 2013
// Revised: August 28th 2013
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
#include "codec_bch.h"

int test_01()
{
	FILE* log = stdout;
	
	int galois_field_degree = 4;
	int code_length = 15;
	int error_correction = 3;
	int times = 100;
	int i = 0;

	bch_encoder_t bch_encoder = bch_encoder_create(log, galois_field_degree, code_length, error_correction);
	bch_decoder_t bch_decoder = bch_decoder_create(ERRORS_CORRECTION_MODE, log, galois_field_degree, code_length, error_correction);

	int frame_size = bch_encoder_get_frame_size(bch_encoder);

	bch_encoder->start(bch_encoder);
	bch_decoder->start(bch_decoder);

L_loop:
	frame_t frame_in = frame_create(frame_size);
	frame_display(frame_in);

	codeword_t codeword_in = bch_encoder->encode(bch_encoder, frame_in);
	codeword_display(codeword_in);

	int codeword_size = bch_encoder_get_codeword_size(bch_encoder);
	codeword_t codeword_out = codeword_create(codeword_size);
	
	/* channel emulation */
	codeword_out = codeword_cpy(codeword_out, codeword_in);
	codeword_display(codeword_out);	

	frame_t frame_out = bch_decoder->decode(bch_decoder, codeword_out, codeword_in);
	frame_display(frame_out);

	frame_destroy(frame_out);
	if(++i != times) goto L_loop;

	bch_encoder->stop(bch_encoder);
	bch_decoder->stop(bch_decoder);

	bch_encoder_destroy(bch_encoder);
	bch_decoder_destroy(bch_decoder);

	return 0;

}