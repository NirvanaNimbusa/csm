//*******************************************************************************
// Title: Communication System Modeler v.1.0
// File: bch_decoder_t.h
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
#ifndef _BCH_DECODER_T_
#define _BCH_DECODER_T_

#include "common.h"

#include "codeword_t.h"
#include "frame_t.h"
#include "bch_decoder_kernel_t.h"

#define SELF bch_decoder_t self
typedef struct bch_decoder bch_decoder_base_t;
typedef bch_decoder_base_t* bch_decoder_t;
struct bch_decoder {
	int galois_field_degree;
	int code_length;
	int inf_symbols_q;
	int error_correction;

	int received_bits_cnt;
	int received_bits_valid_cnt;
	int received_bits_corrupted_cnt;
	int received_codewords_cnt;
	int received_codewords_valid_cnt;
	int received_codewords_corrupted_cnt;
	int generated_frames_cnt;

	FILE* log;
	bch_decoder_kernel_t bch_decoder_kernel;
	int (*start) (SELF);
	int (*stop) (SELF);
	frame_t (*decode) (SELF, codeword_t codeword_out, codeword_t codeword_in);
};

void bch_decoder_init (SELF);
void bch_decoder_deinit (SELF);

bch_decoder_t bch_decoder_create (FILE* log, int galois_field_degree, int code_length, int error_correction);
void bch_decoder_destroy (SELF);

int bch_decoder_start (SELF);
int bch_decoder_stop (SELF);

frame_t bch_decoder_decode (SELF, codeword_t const codeword_out, codeword_t const codeword_in);

int bch_decoder_get_frame_size(SELF);
int bch_decoder_get_codeword_size(SELF);

#undef SELF

#endif