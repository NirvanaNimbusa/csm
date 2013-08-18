//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: cnv_decoder_t.h
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
#ifndef _CNV_DECODER_T_
#define _CNV_DECODER_T_

#include "codeword_t.h"
#include "frame_t.h"
#include "trellis_t.h"
#include "common.h"

#include <stdio.h>

#define SELF cnv_decoder_t self
typedef struct conv_decoder cnv_decoder_base_t;
typedef cnv_decoder_base_t* cnv_decoder_t;
struct conv_decoder {
	FILE* log;
	trellis_t trellis;
	int decoded_sequence_buffer_size;
	int regs_q;
	int skip_q;

	int received_bits_cnt;
	int received_bits_valid_cnt;
	int received_bits_corrupted_cnt;
	int received_codewords_cnt;
	int received_codewords_valid_cnt;
	int received_codewords_corrupted_cnt;
	int generated_frames_cnt;

	int (*start) (SELF);
	int (*stop) (SELF);
	frame_t (*decode) (SELF, codeword_t codeword_out, codeword_t codeword_in);
};

void cnv_decoder_init (SELF);
void cnv_decoder_deinit (SELF);

cnv_decoder_t cnv_decoder_create (FILE* log, int regs_q, int decoded_sequence_buffer_size);
void cnv_decoder_destroy (SELF);

int cnv_decoder_start (SELF);
int cnv_decoder_stop (SELF);

frame_t cnv_decoder_decode (SELF, codeword_t codeword_out, codeword_t codeword_in);
void reset_conv_decoder(SELF);

#undef SELF

typedef struct parents parents_t;
struct parents
{
	trellis_node_t first;
	trellis_node_t second;
};

#endif