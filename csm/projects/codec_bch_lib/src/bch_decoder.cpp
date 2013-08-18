//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: bch_decoder.cpp
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
#include "bch_decoder_t.h"

#include <stdlib.h>

bch_decoder_t bch_decoder_create (bch_decoder_mode_t bch_decoder_mode, FILE* log, int galois_field_degree, int code_length, int error_correction)
{
	bch_decoder_t self = (bch_decoder_t)malloc(sizeof(bch_decoder_base_t));
	if(!self) return NULL;

	self->mode = bch_decoder_mode;
	self->log = log;
	self->galois_field_degree = galois_field_degree;
	self->code_length = code_length;
	self->inf_symbols_q = 0;
	self->error_correction = error_correction;

	bch_decoder_init(self);
	return self;
}

void bch_decoder_init (bch_decoder_t self)
{
	log2(self->log, "bch decoder initialization started\n");
	self->start = bch_decoder_start;
	self->stop = bch_decoder_stop;
	self->decode = bch_decoder_decode;

	self->bch_decoder_kernel = bch_decoder_kernel_create(self->log, self->galois_field_degree,
		self->code_length, self->inf_symbols_q, self->error_correction);

	self->inf_symbols_q = self->bch_decoder_kernel->inf_symbols_q;
}

void bch_decoder_deinit (bch_decoder_t self)
{
	bch_decoder_kernel_destroy(self->bch_decoder_kernel);
	free(self);
}

void bch_decoder_destroy (bch_decoder_t self)
{
	if(!self) return;
	bch_decoder_deinit (self);
}

int bch_decoder_start (bch_decoder_t self)
{
	log2(self->log, "bch decoder started\n");
	self->received_bits_cnt = 0;
	self->received_bits_valid_cnt = 0;
	self->received_bits_corrupted_cnt = 0;
	self->received_codewords_cnt = 0;
	self->received_codewords_valid_cnt = 0;
	self->received_codewords_corrupted_cnt = 0;
	self->generated_frames_cnt = 0;
	return 0;
}

int bch_decoder_stop (bch_decoder_t self)
{
	log(self->log, "\n");
	log(self->log, "... bch decoder stopped\n");

	log(self->log, "... bch decoder settings ...\n");
	log(self->log, "galois field degree: %d\n", self->galois_field_degree);
	log(self->log, "code length: %d\n", self->code_length);
	log(self->log, "information symbols quantity: %d\n", self->inf_symbols_q);
	log(self->log, "error correction property: %d\n", self->error_correction);

	log(self->log, "... bch decoder statistics ...\n");
	log(self->log, "received bits: %d\n", self->received_bits_cnt);
	log(self->log, "received bits (valid): %d\n", self->received_bits_valid_cnt);
	log(self->log, "received bits (corrupted): %d\n", self->received_bits_corrupted_cnt);

	log(self->log, "received codewords: %d\n", self->received_codewords_cnt);
	log(self->log, "received codewords (valid): %d\n", self->received_codewords_valid_cnt);
	log(self->log, "received codewords (corrupted): %d\n", self->received_codewords_corrupted_cnt);
	log(self->log, "founded error codewords: %d\n", bch_decoder_kernel_get_founded_errors(self->bch_decoder_kernel));
	log(self->log, "recovered bits: %d\n", bch_decoder_kernel_get_recovered_bits(self->bch_decoder_kernel));

	log(self->log, "generated frames: %d\n", self->generated_frames_cnt);
	return 0;
}

frame_t bch_decoder_decode (bch_decoder_t self, codeword_t codeword_out, codeword_t codeword_in)
{
	log2(self->log, "bch decoding started\n");
	int frame_size = bch_decoder_get_frame_size(self);

	self->received_bits_cnt += codeword_out->xsize;
	self->received_codewords_cnt++;

	if(codeword_cmp(codeword_out, codeword_in))
	{
		self->received_bits_valid_cnt += codeword_out->xsize;
		self->received_codewords_valid_cnt++;
	}
	else
	{
		int invalid_bits_q = codeword_find_invalid_bits(codeword_out, codeword_in);
		self->received_bits_corrupted_cnt += invalid_bits_q;
		self->received_codewords_corrupted_cnt++;
	}

	if(self->mode == ERRORS_CORRECTION_MODE)
	{
		frame_t frame = bch_decoder_kernel_decode(self->bch_decoder_kernel, codeword_out, codeword_in);
		self->generated_frames_cnt++;

		codeword_destroy(codeword_out);
		codeword_destroy(codeword_in);
		log2(self->log, "bch decoding finished\n");

		return frame;
	}
	else if(self->mode == ERRORS_ERASE_CORRECTION_MODE)
	{
		int erase_errors_quantity = self->bch_decoder_kernel->bch_decoder_kernel_vars->erase_errors_q;
		int corrected_errors_quantity_f = 0;
		int corrected_errors_quantity_s = 0;

		/* Замена стерных символов нулями. */
		/* ПРИМЕЧАНИЕ: Стирания уже сгенерированы каналом. Но для понимания работы алгоритма выполняется земена. */
		for (int i = codeword_out->xsize - erase_errors_quantity; i < codeword_out->xsize; i++)
			codeword_out->xcodeword[i] = 0;		

		/* Докодирование полученного слова. */
		frame_t frame_f = bch_decoder_kernel_decode(self->bch_decoder_kernel, codeword_out, codeword_in);
		
		/* Количество ошибок на нестертых позициях. */
		corrected_errors_quantity_f = self->bch_decoder_kernel->bch_decoder_kernel_vars->corrected_errors_nonerased_positions;

		/* Замена стерных символов единицами. */
		for (int i = codeword_out->xsize - erase_errors_quantity; i < codeword_out->xsize; i++)
			codeword_out->xcodeword[i] = 1;	

		/* Докодирование полученного слова. */
		frame_t frame_s = bch_decoder_kernel_decode(self->bch_decoder_kernel, codeword_out, codeword_in);

		/* Количество ошибок на нестертых позициях. */
		corrected_errors_quantity_s = self->bch_decoder_kernel->bch_decoder_kernel_vars->corrected_errors_nonerased_positions;

		/* Выбор в пользу декодирования с наименьшим числом ошибок на настертых позициях. */
		if(corrected_errors_quantity_f <= corrected_errors_quantity_s)
		{
			frame_destroy(frame_s);
			self->generated_frames_cnt++;
			return frame_f;
		}
		else
		{
			frame_destroy(frame_f);
			self->generated_frames_cnt++;
			return frame_s;
		}
	}
	return NULL;
}

int bch_decoder_get_frame_size(bch_decoder_t self)
{
	return self->inf_symbols_q;
}

int bch_decoder_get_codeword_size(bch_decoder_t self)
{
	return self->code_length;
}

void bch_decoder_set_erase_errors_q(bch_decoder_t self, int erase_errors_q)
{
	self->bch_decoder_kernel->bch_decoder_kernel_vars->erase_errors_q = erase_errors_q;
}
