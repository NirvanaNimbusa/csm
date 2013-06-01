//*******************************************************************************
// Title: Communication System Modeler v.1.0
// File: test_02.cpp
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
#include "common.h"
#include "test_engine.h"
#include "codec_bch.h"

#include <stdlib.h>

void print_data(int* data, int data_size)
{
	for (int i = 0; i < data_size; i++)
	{
		printf("%d", data[i]);
	}
	printf("\n");
}

int cmp_data(int* f_data, int* s_data, int data_size)
{
	for (int i = 0; i < data_size; i++)
		if (f_data[i] != s_data[i])
			return 0;
	return 1;
}

int test_02()
{
	FILE* log = stdout;
/************************************************************************/
/* ��������� ��������� ���������.                                       */
/************************************************************************/
	int data_size = (int)1e2;
	int* data_in = (int*)calloc(data_size, sizeof(int));
	int* data_out = NULL;

/************************************************************************/
/* ��������� ���-������                                                 */
/************************************************************************/
	int galois_field_degree = 4;
	int code_length = 15;
	int error_correction = 3;

/************************************************************************/
/* �������� ����������� �����������.                                    */
/************************************************************************/
	bch_encoder_t bch_encoder = bch_encoder_create(log, galois_field_degree, code_length, error_correction);
	bch_decoder_t bch_decoder = bch_decoder_create(log, galois_field_degree, code_length, error_correction);

	/* ��������� ����� ������ ������, ������� ����� ���������� ���-�����. */
	int frame_size = bch_encoder_get_frame_size(bch_encoder);
	transmitter_t transmitter = transmitter_create(log, frame_size, data_in, data_size);
	receiver_t receiver = receiver_create(log, frame_size, data_size);

/************************************************************************/
/* ������ ����������� �����������.                                      */
/************************************************************************/
	transmitter->start(transmitter);
	receiver->start(receiver);

	bch_encoder->start(bch_encoder);
	bch_decoder->start(bch_decoder);

/************************************************************************/
/* ������ ����� �������������.                                          */
/************************************************************************/
L_loop:
	/* ��������� ������ ������ �� �����������. */
	frame_t frame_in = transmitter->transmit_frame(transmitter);
	if (frame_in == NULL) goto L_Stop;

	frame_display(frame_in);

	/* ����������� ������ ������ ���-������� � ��������� �������� ����� ���-����. */
	codeword_t codeword_in = bch_encoder->encode(bch_encoder, frame_in);
	frame_t frame_in_save = frame_create(frame_size);
	frame_in_save = frame_cpy(frame_in_save, frame_in);
	codeword_display(codeword_in);

	/* �������� �������� �������� ����� �� ������. */
	codeword_t codeword_out = codeword_create(codeword_in->xsize);
	codeword_out = codeword_cpy(codeword_out, codeword_in);
	codeword_display(codeword_out);	

	/* ������������� �������� ����� ���-��������� � ��������� ������ ������. */
	frame_t frame_out = bch_decoder->decode(bch_decoder, codeword_out, codeword_in);
	frame_display(frame_out);

	/* ����� ������ ������ ����������. */
	receiver->receive_frame(receiver, frame_out, frame_in_save);
	goto L_loop;

L_Stop:
/************************************************************************/
/* ������� ����������� �����������.                                     */
/************************************************************************/
	transmitter->stop(transmitter);
	receiver->stop(receiver);
	bch_encoder->stop(bch_encoder);
	bch_decoder->stop(bch_decoder);

	/* �������� ���������� ������������ ������ �� ���������. */
	data_out = receiver_get_received_data(receiver);

	/* �������� ������������ �������� ������. */
	if (cmp_data(data_in, data_out, data_size))
		printf("test: OK\n");
	else
		printf("test: FAIL\n");

	free(data_in);

/************************************************************************/
/* ����������� ����������� �����������.                                 */
/************************************************************************/
	transmitter_destroy(transmitter);
	receiver_destroy(receiver);
	bch_encoder_destroy(bch_encoder);
	bch_decoder_destroy(bch_decoder);

	return 0;

}