//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: bch_decoder_kernel.cpp
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
#include "bch_decoder_kernel_t.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

bch_decoder_kernel_t bch_decoder_kernel_create
(	
	FILE* log, int galois_field_degree, int code_length, int inf_symbols_q, int error_correction)
{
	bch_decoder_kernel_t self = (bch_decoder_kernel_t)malloc(sizeof(bch_decoder_kernel_base_t));
	if(!self) return NULL;

	self->log = log;
	self->galois_field_degree = galois_field_degree;
	self->code_length = code_length;
	self->inf_symbols_q = inf_symbols_q;
	self->error_correction = error_correction;

	bch_decoder_kernel_init(self);
	return self;
}

void bch_decoder_kernel_init (bch_decoder_kernel_t self)
{
	log2(self->log, "bch decoder kernel initialization started\n");

	int galois_field_degree = self->galois_field_degree; /* Степень поля Галуа. */
	int code_length = self->code_length; /* Длина кодового слова. */
	int error_correction = self->error_correction; /* Количество исправляемых ошибок. */

	/* Создание переменных ядра БЧХ-кодера. */
	self->bch_decoder_kernel_vars = bch_codec_kernel_vars_create();
	bch_codec_kernel_vars_t vars = self->bch_decoder_kernel_vars;

	vars = check_and_set_code_params(vars, galois_field_degree, code_length, error_correction);
	vars = generate_primitive_polynom(vars);        
	vars = generate_evariste_galois_field(vars);
	vars = generate_generator_polynom(vars);

	/* Установление вычисленного значения количества информационных символов. */
	self->inf_symbols_q = self->bch_decoder_kernel_vars->inf_symbols_q;
}

void bch_decoder_kernel_destroy (bch_decoder_kernel_t self)
{
	if(!self) return;
	bch_decoder_kernel_deinit (self);
}

void bch_decoder_kernel_deinit (bch_decoder_kernel_t self)
{
	bch_codec_kernel_vars_destroy(self->bch_decoder_kernel_vars);
	free(self);
}


/************************************************************************
 * Encoder/bch_decoder for binary BCH codes
 *
 * galois_field_degree = order of the Galois field GF(2**galois_field_degree) 
 * code_length = 2**galois_field_degree - 1 = size of the multiplicative group of GF(2**galois_field_degree)
 * code_length = code_length of the BCH encode
 * error_correction = error correcting capability (max. no. of errors the encode corrects)
 * code_distance = 2*error_correction + 1 = designed min. distance = no. of consecutive roots of generator_polynomial(x) + 1
 * inf_symbols_q = code_length - deg(generator_polynomial(x)) = dimension (no. of information bits/codeword) of the encode
 * p[] = coefficients of a primitive polynomial used to generate GF(2**galois_field_degree)
 * generator_polynomial[] = coefficients of the generator polynomial, generator_polynomial(x)
 * alpha_to [] = log table of GF(2**galois_field_degree) 
 * index_of[] = antilog table of GF(2**galois_field_degree)
 * data[] = information bits = coefficients of data polynomial, i(x)
 * bb[] = coefficients of redundancy polynomial x^(code_length-inf_symbols_q) i(x) modulo generator_polynomial(x)
 * number_of_errors = number of errors 
 * error_positions[] = error positions 
 * received_codeword[] = coefficients of the received polynomial 
 * decerror = number of decoding errors (in _message_ positions) 
 *
 */
frame_t bch_decoder_kernel_decode (bch_decoder_kernel_t self, codeword_t codeword_out, codeword_t codeword_in)
{
	bch_codec_kernel_vars_t bch_decoder_kernel_vars = self->bch_decoder_kernel_vars;
	bch_decoder_kernel_vars->number_of_errors = bch_decoder_kernel_vars->error_correction;

	//!error_positions are not used
	for (int i = 0; i < bch_decoder_kernel_vars->number_of_errors; i++)
		bch_decoder_kernel_vars->error_positions[i] = i; //! rand()

	//copy
	for (int i = 0; i < bch_decoder_kernel_vars->code_length; i++)
		bch_decoder_kernel_vars->received_codeword[i] = codeword_out->xcodeword[i];

	printf_d("r(x) = ");
	for (int i = 0; i < bch_decoder_kernel_vars->code_length; i++) {
		printf_d("%1d", bch_decoder_kernel_vars->received_codeword[i]);
		if (i && ((i % 50) == 0))
			printf_d("\n");
	}
	printf_d("\n");

	decode_bch(bch_decoder_kernel_vars);             /* DECODE received codeword recv[] */

	/*
	 * print out original and decoded data
	 */
	//printf_d("Results:\code_length");
	//printf_d("original data  = ");
	//for (i = 0; i < bch_decoder_kernel_vars->inf_symbols_q; i++) {
	//	printf_d("%1d", bch_decoder_kernel_vars->data[i]);
	//	if (i && ((i % 50) == 0))
	//		printf_d("\code_length");
	//}

	printf_d("decoded data:\n");
	for (int i = bch_decoder_kernel_vars->code_length - bch_decoder_kernel_vars->inf_symbols_q;
		i < bch_decoder_kernel_vars->code_length; i++) {
		printf_d("%1d", bch_decoder_kernel_vars->received_codeword[i]);
		if ((i-bch_decoder_kernel_vars->code_length+bch_decoder_kernel_vars->inf_symbols_q) &&
			(((i-bch_decoder_kernel_vars->code_length+bch_decoder_kernel_vars->inf_symbols_q) % 50) == 0))
			printf_d("\n");
	}
	printf_d("\n");

	/*
	 * DECODING ERRORS? we compare only the data portion
	 */
	//for (i = bch_decoder_kernel_vars->code_length - bch_decoder_kernel_vars->inf_symbols_q; i < bch_decoder_kernel_vars->code_length; i++)
	//	if (bch_decoder_kernel_vars->data[i - bch_decoder_kernel_vars->code_length + bch_decoder_kernel_vars->inf_symbols_q]
	//	!= bch_decoder_kernel_vars->received_codeword[i])
	//		bch_decoder_kernel_vars->decerror++;
	//if (bch_decoder_kernel_vars->decerror)
	//   printf_d("There were %code_distance decoding errors in message positions\code_length", bch_decoder_kernel_vars->decerror);
	//else
	//   printf_d("Succesful decoding\code_length");

	frame_t frame = frame_create(bch_decoder_kernel_vars->inf_symbols_q);
	unsigned int ind = bch_decoder_kernel_vars->code_length - bch_decoder_kernel_vars->inf_symbols_q;
	for (int j = 0, i = ind; i < bch_decoder_kernel_vars->code_length; i++, j++)
		frame->xframe[j] = bch_decoder_kernel_vars->received_codeword[i];
	return frame;
}

unsigned int bch_decoder_kernel_get_founded_errors(bch_decoder_kernel_t self)
{
	return self->bch_decoder_kernel_vars->founded_errors;
}

unsigned int bch_decoder_kernel_get_recovered_bits(bch_decoder_kernel_t self)
{
	return self->bch_decoder_kernel_vars->corrected_errors;
}