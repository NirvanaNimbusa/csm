//*******************************************************************************
// Title: Communication System Modeler v.1.0
// File: bch_encoder_kernel.cpp
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
#include "bch_encoder_kernel_t.h"

#include <stdlib.h>
#include <stdio.h>

bch_encoder_kernel_t bch_encoder_kernel_create (FILE* log, int galois_field_degree, int code_length, int inf_symbols_q, int error_correction)
{
	bch_encoder_kernel_t self = (bch_encoder_kernel_t)malloc(sizeof(bch_encoder_kernel_base_t));
	if(!self) return NULL;

	self->log = log;
	self->galois_field_degree = galois_field_degree;
	self->code_length = code_length;
	self->inf_symbols_q = inf_symbols_q;
	self->error_correction = error_correction;

	bch_encoder_kernel_init(self);
	return self;
}

void bch_encoder_kernel_init (bch_encoder_kernel_t self)
{
	log2(self->log, "bch encoder kernel initialization started\n");

	int galois_field_degree = self->galois_field_degree; /* Степень поля Галуа. */
	int code_length = self->code_length; /* Длина кодового слова. */
	int error_correction = self->error_correction; /* Количество исправляемых ошибок. */

	/* Создание переменных ядра БЧХ-кодера. */
	self->bch_encoder_kernel_vars = bch_codec_kernel_vars_create();
	bch_codec_kernel_vars_t vars = self->bch_encoder_kernel_vars;
	
	vars = check_and_set_code_params(vars, galois_field_degree, code_length, error_correction);
	vars = generate_primitive_polynom(vars);
	vars = generate_evariste_galois_field(vars);
	vars = generate_generator_polynom(vars);

	/* Установление вычисленного значения количества информационных символов. */
	self->inf_symbols_q = self->bch_encoder_kernel_vars->inf_symbols_q;
}

void bch_encoder_kernel_destroy (bch_encoder_kernel_t self)
{
	if(!self) return;
	bch_encoder_kernel_deinit (self);
}

void bch_encoder_kernel_deinit (bch_encoder_kernel_t self)
{
	bch_codec_kernel_vars_destroy(self->bch_encoder_kernel_vars);
	free(self);
}

codeword_t bch_encoder_kernel_encode (bch_encoder_kernel_t self, frame_t const frame)
{
	log2(self->log, "bch encoder kernel encoding start\n");
	bch_codec_kernel_vars_t bch_encoder_kernel_vars = self->bch_encoder_kernel_vars;
	
	int codeword_length = bch_encoder_kernel_vars->code_length;
	codeword_t codeword = codeword_create(codeword_length);

	printf_d("source data:\n");

	int i;
	for (i = 0; i < frame->xsize; i++)
		bch_encoder_kernel_vars->data[i] = frame->xframe[i];
	for (i = 0; i < bch_encoder_kernel_vars->inf_symbols_q; i++)
		printf_d("%d", bch_encoder_kernel_vars->data[i]);
	printf_d("\n");

	encode_bch(bch_encoder_kernel_vars);

	/*
	 * received_codeword[] are the coefficients of c(x) = x**(code_length-inf_symbols_q)*data(x) + b(x)
	 */
	for (i = 0; i < bch_encoder_kernel_vars->code_length - bch_encoder_kernel_vars->inf_symbols_q; i++)
		bch_encoder_kernel_vars->received_codeword[i] = bch_encoder_kernel_vars->bb[i];
	for (i = 0; i < bch_encoder_kernel_vars->inf_symbols_q; i++)
		bch_encoder_kernel_vars->received_codeword[i + bch_encoder_kernel_vars->code_length
		- bch_encoder_kernel_vars->inf_symbols_q] = bch_encoder_kernel_vars->data[i];

	printf_d("code polynomial: c(x) = ");
	for (i = 0; i < bch_encoder_kernel_vars->code_length; i++) 
	{
		printf_d("%1d", bch_encoder_kernel_vars->received_codeword[i]);
		if (i && ((i % 50) == 0))
			printf_d("\n");
	}
	printf_d("\n");

	for (i = 0; i < bch_encoder_kernel_vars->code_length; i++)
		codeword->xcodeword[i] = bch_encoder_kernel_vars->received_codeword[i];

	return codeword;
}