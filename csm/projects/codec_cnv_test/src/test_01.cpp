//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: test_01.cpp
// Author: Pavel Morozkin
// Date: August 17th 2013
// Revised: August 17th 2013
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
#include "common.h"
#include "codec_cnv.h"

int test_01()
{
	FILE* log = stdout;
	int frame_length = 5;
	int regs_q = 2; /* ! do not change */
	int codeword_length = frame_length * regs_q;
	int decoded_seq_buf_size = frame_length * 1 + 1;

	int times = 10;
	int i = 0;

	cnv_encoder_t cnv_encoder = cnv_encoder_create(log, regs_q, codeword_length);
	cnv_decoder_t cnv_decoder = cnv_decoder_create(log, regs_q, decoded_seq_buf_size);
	
	cnv_encoder->start(cnv_encoder);
	cnv_decoder->start(cnv_decoder);

L_Loop:
	frame_t frame_in = frame_create(frame_length);
	frame_display(frame_in);

	codeword_t codeword_in = cnv_encoder->encode(cnv_encoder, frame_in);
	codeword_display(codeword_in);

	/* channel emulation */
	codeword_t codeword_out = codeword_create(codeword_length);
	codeword_out = codeword_cpy(codeword_out, codeword_in);
	codeword_display(codeword_out);

	frame_t frame_out = cnv_decoder->decode(cnv_decoder, codeword_out, codeword_in);
	frame_display(frame_out);

	frame_destroy(frame_out);
	if(++i != times) goto L_Loop;

	cnv_encoder->stop(cnv_encoder);
	cnv_decoder->stop(cnv_decoder);

	cnv_encoder_destroy(cnv_encoder);
	cnv_decoder_destroy(cnv_decoder);

	return 0;

}