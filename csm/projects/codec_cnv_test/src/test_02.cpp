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
#include "codec_cnv.h"

#include <stdlib.h>

int cmp_data(int* f_data, int* s_data, int data_size)
{
	for (int i = 0; i < data_size; i++)
		if (f_data[i] != s_data[i])
			return 0;
	return 1;
}

int test_02()
{
	FILE* flog = stdout;

/************************************************************************/
/* Параметры тестового окружения.                                       */
/************************************************************************/
	int frame_size = 5;
	int data_size = (int)1e2;
	int* data_in = (int*)calloc(data_size, sizeof(int));
	int* data_out = NULL;

/************************************************************************/
/* Параметры сверточного кодера.                                        */
/************************************************************************/
	int regs_q = 2; /* ! do not change */
	int codeword_length = frame_size * regs_q;
	int decoded_seq_buf_size_frames = 10;
	int decoded_seq_buf_size = frame_size * decoded_seq_buf_size_frames + 1;
	int skip_q = decoded_seq_buf_size_frames;


/************************************************************************/
/* Параметры промежуточного буфера                                      */
/************************************************************************/
	int frame_fifo_size = 100;

	int skip_q_cnt = 0;

/************************************************************************/
/* Создание программных компонентов.                                    */
/************************************************************************/
	transmitter_t transmitter = transmitter_create(flog, frame_size, data_in, data_size);
	receiver_t receiver = receiver_create(flog, frame_size, data_size);

	cnv_encoder_t cnv_encoder = cnv_encoder_create(flog, regs_q, codeword_length);
	cnv_decoder_t cnv_decoder = cnv_decoder_create(flog, regs_q, decoded_seq_buf_size);

	frame_fifo_t frame_fifo = frame_fifo_create(frame_fifo_size, frame_size);

/************************************************************************/
/* Запуск программных компонентов.                                      */
/************************************************************************/
	transmitter->start(transmitter);
	receiver->start(receiver);

	cnv_encoder->start(cnv_encoder);
	cnv_decoder->start(cnv_decoder);

/************************************************************************/
/* Запуск цикла моделирования.                                          */
/************************************************************************/
L_Loop:
	frame_t frame_in = transmitter->transmit_frame(transmitter);
	if (frame_in == NULL){
		log(flog, "tx already sent all data, transmit dummy codeword\n");
		goto L_Prepare;
	}

	frame_display(frame_in);
	frame_fifo = frame_fifo_put(frame_fifo, frame_in);

	codeword_t codeword_in = cnv_encoder->encode(cnv_encoder, frame_in);
	codeword_display(codeword_in);
	goto L_Skip_Prepare;

L_Prepare:
	codeword_in = codeword_create(codeword_length);

L_Skip_Prepare:
	/* channel emulation */
	codeword_t codeword_out = codeword_create(codeword_length);
	codeword_out = codeword_cpy(codeword_out, codeword_in);
	codeword_display(codeword_out);

	frame_t frame_out = cnv_decoder->decode(cnv_decoder, codeword_out, codeword_in);
	frame_display(frame_out);

	if(skip_q_cnt != skip_q){
		skip_q_cnt++;
		frame_destroy(frame_out);
		goto L_Loop;
	}

	frame_in = frame_fifo_get(frame_fifo);
	if(frame_in == NULL){
		log(flog, "frame fifo is empty, prepare for stopping\n");
		frame_destroy(frame_out);
		goto L_Stop;
	}

	frame_display(frame_in);
	receiver->receive_frame(receiver, frame_out, frame_in);

	goto L_Loop;

L_Stop:
/************************************************************************/
/* Останов программных компонентов.                                     */
/************************************************************************/
	transmitter->stop(transmitter);
	receiver->stop(receiver);
	cnv_encoder->stop(cnv_encoder);
	cnv_decoder->stop(cnv_decoder);

	data_out = receiver_get_received_data(receiver);

	if (cmp_data(data_in, data_out, data_size))
		printf("test: OK\n");
	else
		printf("test: FAIL\n");

	//print_data(data_in, data_size);
	//print_data(data_out, data_size);

	free(data_in);

/************************************************************************/
/* Уничтожение программных компонентов.                                 */
/************************************************************************/
	transmitter_destroy(transmitter);
	receiver_destroy(receiver);
	cnv_encoder_destroy(cnv_encoder);
	cnv_decoder_destroy(cnv_decoder);

	frame_fifo_destroy(frame_fifo);

	return 0;

}