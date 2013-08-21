//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: bch_codec_kernel.h
// Author: Pavel Morozkin
// Date: August 21th 2013
// Revised: August 21th 2013
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
#ifndef _BCH_CODEC_KERNEL_
#define _BCH_CODEC_KERNEL_

#include "common.h"

typedef struct bch_codec_kernel_vars bch_codec_kernel_vars_base_t;
typedef bch_codec_kernel_vars_base_t* bch_codec_kernel_vars_t;

struct bch_codec_kernel_vars
{
	int             galois_field_degree, code_length_max, code_length, inf_symbols_q, error_correction, code_distance;
	int             primitive_polynomial[21];
	int             alpha_to[1048576], index_of[1048576], generator_polynomial[548576];
	int             received_codeword[1048576], data[1048576], bb[548576];
	int             seed;
	int             number_of_errors, error_positions[1024], decerror; //= 0;
	unsigned int	founded_errors;
	unsigned int	corrected_errors;
	unsigned int	erase_errors_q;
	unsigned int	corrected_errors_nonerased_positions;
};

bch_codec_kernel_vars_t bch_codec_kernel_vars_create ();
void bch_codec_kernel_vars_destroy (bch_codec_kernel_vars_t self);

bch_codec_kernel_vars_t
check_and_set_code_params(bch_codec_kernel_vars_t vars, int galois_field_degree, int code_length, int error_correction);
bch_codec_kernel_vars_t generate_primitive_polynom(bch_codec_kernel_vars_t vars);
bch_codec_kernel_vars_t generate_evariste_galois_field(bch_codec_kernel_vars_t vars);
bch_codec_kernel_vars_t generate_generator_polynom(bch_codec_kernel_vars_t vars);

void encode_bch(bch_codec_kernel_vars_t vars);
void decode_bch(bch_codec_kernel_vars_t vars);

#endif