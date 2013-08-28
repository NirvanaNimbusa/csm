//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: kernel_cnv_mode.cpp
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
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include "kernel.h"

int kernel_run_cnv_mode
	(
	int galois_field_degree,
	int bch_code_length,
	int error_correction,
	int decoded_seq_buf_size_frames,
	double ber,
	char* out_file_postfix,
	char* input_file_name,
	int gui_progress)
{
	FILE* flog = stdout;
	log(flog, "Current mode: use of CNV codec only via BER parametrized channel\n");
	log(flog, "Simulation kernel settings:\n");
	log(flog, "Galois field degree: %d\n",							galois_field_degree);
	log(flog, "BCH code length: %d\n",								bch_code_length);
	log(flog, "Error correction property: %d\n",					error_correction);
	log(flog, "Size of decoded buffer (CNV decoder): %d\n",			decoded_seq_buf_size_frames);
	log(flog, "BER (Bit Error Rate): %.2f\n",						ber);
	log(flog, "Output file postfix: %s\n",							out_file_postfix);
	log(flog, "Input file name: %s\n",								input_file_name);

	/************************************************************************/
	/* Параметры тестового окружения.                                       */
	/************************************************************************/
	int* data_in = get_file_data(input_file_name);
	int data_size = get_file_size(input_file_name) * CHAR_BIT;
	//print_data(data_in, data_size);
	int* data_out = NULL;

	int use_bch_codec = 0;

	/************************************************************************/
	/* Параметры ДСК-канала                                                 */
	/************************************************************************/
	//double ber = 0;//2*1e-5;

	/************************************************************************/
	/* Технологические переменные.                                          */
	/************************************************************************/
	int skip_q_cnt = 0;
	int percent_precision = gui_progress ? (int)1e3 : 5;
	int sent_bits_cnt = 0;

	/************************************************************************/
	/* Создание программных компонентов.                                    */
	/************************************************************************/
	int frame_size = 10;
	transmitter_t transmitter = transmitter_create(flog, frame_size, data_in, data_size);
	receiver_t receiver = receiver_create(flog, frame_size, data_size);

	/************************************************************************/
	/* Параметры сверточного кодера.                                        */
	/************************************************************************/
	int regs_q = 2; /* ! do not change */
	int codeword_length = frame_size * regs_q;
	//int decoded_seq_buf_size_frames = 2;
	int decoded_seq_buf_size = frame_size * decoded_seq_buf_size_frames + 1;
	int skip_q = decoded_seq_buf_size_frames;

	/************************************************************************/
	/* Создание программных компонентов.                                    */
	/************************************************************************/
	cnv_encoder_t cnv_encoder = cnv_encoder_create(flog, regs_q, codeword_length);
	cnv_decoder_t cnv_decoder = cnv_decoder_create(flog, regs_q, decoded_seq_buf_size);

	/************************************************************************/
	/* Создание программных компонентов.                                    */
	/************************************************************************/
	channel_bs_t channel_bs = channel_bs_create(flog, ber);

	/************************************************************************/
	/* Параметры промежуточных буферов                                      */
	/************************************************************************/
	int frame_fifo_size = decoded_seq_buf_size_frames + 1;

	/************************************************************************/
	/* Создание промежуточных буферов.                                      */
	/************************************************************************/
	frame_fifo_t frame_fifo = frame_fifo_create(frame_fifo_size, frame_size);

	/************************************************************************/
	/* Запуск программных компонентов.                                      */
	/************************************************************************/
	log(flog, "Simulation started\n");
	transmitter->start(transmitter);
	receiver->start(receiver);
	cnv_encoder->start(cnv_encoder);
	cnv_decoder->start(cnv_decoder);
	channel_bs->start(channel_bs);

	/************************************************************************/
	/* Запуск цикла моделирования.                                          */
	/************************************************************************/
	int sent_frames = 0;
	int frames_to_sent = data_size/frame_size;
	int send_bits_all = data_size+decoded_seq_buf_size_frames*frame_size;
L_Loop:
	/*if (!(sent_bits_cnt % percent_precision) && !gui_progress)
		printf("\rProgress: %.0f%%", ((float)sent_bits_cnt/
		(float)send_bits_all) * 100);
	else if (!(sent_bits_cnt % percent_precision))
		printf("Progress: %.0f%%\n", ((float)sent_bits_cnt/
		(float)send_bits_all) * 100);*/
	loadbar(sent_frames++, frames_to_sent, 50);
	sent_bits_cnt += frame_size;

	/* Генерация фрейма данных передатчиком. */
	frame_t frame_in = transmitter->transmit_frame(transmitter);
	if (frame_in == NULL) goto L_Dummy_Generation;

	frame_display(frame_in);

	/* Запись сгенерированного фрейма данных в промежуточный буфер. */
	frame_fifo = frame_fifo_put(frame_fifo, frame_in);
	
	/* Кодирование фрейма данных сверточным кодером и получение кодового слова сверточного кода.*/
	codeword_t cnv_codeword_in = cnv_encoder->encode(cnv_encoder, frame_in);
	goto L_Skip_Dummy_Generation;

L_Dummy_Generation:
	cnv_codeword_in = codeword_create(cnv_encoder->codeword_length);

L_Skip_Dummy_Generation:
	codeword_display(cnv_codeword_in);

	/* Передача кодового слова сверточного кода по каналу. */
	codeword_t cnv_codeword_out = channel_bs->transfer(channel_bs, cnv_codeword_in);
	codeword_display(cnv_codeword_out);	

	/* Декодирование кодового слова сверточного кода и получение фрейма данных. */
	frame_t frame_out = cnv_decoder->decode(cnv_decoder, cnv_codeword_out, cnv_codeword_in);
	frame_display(frame_out);

	/* Проверка счетчика пропусков. */
	if(skip_q_cnt != skip_q) { 
		skip_q_cnt++; 
		goto L_Loop;
	}

	/* Извлечение из промежуточного буфера фрейма данных. */
	frame_in = frame_fifo_get(frame_fifo);
	frame_display(frame_in);

	/* Прием фрейма данных и анализ его корректности. */
	receiver->receive_frame(receiver, frame_out, frame_in);

	/* Проверка наличия фреймов в буфере. */
	if(frame_fifo_is_empty(frame_fifo))
		goto L_Stop;
	else 
		goto L_Loop;

	/************************************************************************/
	/* Останов программных компонентов.                                     */
	/************************************************************************/
L_Stop:
	transmitter->stop(transmitter);
	receiver->stop(receiver);
	cnv_encoder->stop(cnv_encoder);
	cnv_decoder->stop(cnv_decoder);
	channel_bs->stop(channel_bs);
	log(flog, "Simulation stopped\n");

	/************************************************************************/
	/* Проверка корректности принятых данных.                               */
	/************************************************************************/
	data_out = receiver_get_received_data(receiver);
	
	printf("\n");
	if (cmp_data(data_in, data_out, data_size))
		printf("Comparison test: OK\n");
	else
		printf("Comparison test: FAIL\n");

	free(data_in);

	out_file_postfix = extend_out_file_postfix(
		out_file_postfix,
		galois_field_degree,
		bch_code_length,
		error_correction,
		decoded_seq_buf_size,
		ber);
	put_file_data(data_out, input_file_name, out_file_postfix);

	//print_data(data_in, data_size);
	//print_data(data_out, data_size);

	/************************************************************************/
	/* Уничтожение программных компонентов.                                 */
	/************************************************************************/
	transmitter_destroy(transmitter);
	receiver_destroy(receiver);
	cnv_encoder_destroy(cnv_encoder);
	cnv_decoder_destroy(cnv_decoder);
	channel_bs_destroy(channel_bs);
	frame_fifo_destroy(frame_fifo);
#ifdef _DEBUG	
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}