//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: kernel_bch_mode.cpp
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
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include "kernel.h"

int kernel_run_bch_ber_ch_mode
(
	int galois_field_degree,
	int bch_code_length,
	int error_correction,
	int decoded_seq_buf_size_frames,
	double ber,
	char* out_file_postfix,
	char* input_file_name,
	int gui_progress
)
{
	FILE* flog = stdout;
	log(flog, "Current mode: use of BCH codec only via BER parametrized channel\n");
	log(flog, "Simulation kernel settings:\n");
	log(flog, "Galois field degree: %d\n",							galois_field_degree);
	log(flog, "BCH code length: %d\n",								bch_code_length);
	log(flog, "Error correction property: %d\n",					error_correction);
	log(flog, "BER (Bit Error Rate): %.2f\n",						ber);
	log(flog, "Output file postfix: %s\n",							out_file_postfix);
	log(flog, "Input file name: %s\n",								input_file_name);

	/************************************************************************/
	/* Параметры тестового окружения.                                       */
	/************************************************************************/
	char* file_path = input_file_name;
	int* data_in = get_file_data(file_path);
	int data_size = get_file_size(file_path) * CHAR_BIT;
	//print_data(data_in, data_size);
	int* data_out = NULL;

	/************************************************************************/
	/* Параметры БЧХ-кодера                                                 */
	/************************************************************************/
	//int galois_field_degree = 4;
	//int bch_code_length = 15;
	//int error_correction = 3;

	/************************************************************************/
	/* Параметры ДСК-канала                                                 */
	/************************************************************************/
	//double ber = 0;//2*1e-5;
	int random_errors_quantity = 5;
	int erase_errors_quantity = 2;

	/************************************************************************/
	/* Создание программных компонентов.                                    */
	/************************************************************************/
	bch_encoder_t bch_encoder = bch_encoder_create(flog, galois_field_degree, bch_code_length, error_correction);
	bch_decoder_t bch_decoder = bch_decoder_create(ERRORS_CORRECTION_MODE, flog, galois_field_degree, bch_code_length, error_correction);

	/* Получение длины фрейма данных, который может кодировать БЧХ-кодер. */
	int bch_frame_size = bch_encoder_get_frame_size(bch_encoder);
	int bch_codeword_size = bch_encoder_get_codeword_size(bch_encoder);

	/************************************************************************/
	/* Создание программных компонентов.                                    */
	/************************************************************************/
	transmitter_t transmitter = transmitter_create(flog, bch_frame_size, data_in, data_size);
	receiver_t receiver = receiver_create(flog, bch_frame_size, data_size);

	/************************************************************************/
	/* Создание программных компонентов.                                    */
	/************************************************************************/
	channel_bs_t channel_bs = channel_bs_create(flog, ber);

	/************************************************************************/
	/* Параметры промежуточных буферов                                      */
	/************************************************************************/
	int bch_frame_fifo_size = decoded_seq_buf_size_frames + 1;
	int bch_codeword_fifo_size = decoded_seq_buf_size_frames + 1;

	/************************************************************************/
	/* Создание промежуточных буферов.                                      */
	/************************************************************************/
	frame_fifo_t bch_frame_fifo = frame_fifo_create(bch_frame_fifo_size, bch_frame_size);
	codeword_fifo_t bch_codeword_fifo = codeword_fifo_create(bch_codeword_fifo_size, bch_codeword_size);

	/************************************************************************/
	/* Запуск программных компонентов.                                      */
	/************************************************************************/
	log(flog, "Simulation started\n");
	transmitter->start(transmitter);
	receiver->start(receiver);
	bch_encoder->start(bch_encoder);
	bch_decoder->start(bch_decoder);
	channel_bs->start(channel_bs);

	/************************************************************************/
	/* Запуск цикла моделирования.                                          */
	/************************************************************************/
	int sent_frames = 0;
	int frames_to_sent = data_size/bch_frame_size;
	int send_bits_all = data_size+decoded_seq_buf_size_frames*bch_frame_size;
L_Loop:
	/*if (!(sent_bits_cnt % percent_precision) && !gui_progress)
		printf("\rProgress: %.0f%%", ((float)sent_bits_cnt/
		(float)send_bits_all) * 100);
	else if (!(sent_bits_cnt % percent_precision))
		printf("Progress: %.0f%%\n", ((float)sent_bits_cnt/
		(float)send_bits_all) * 100);*/
	loadbar(sent_frames++, frames_to_sent, 50);

	/* Генерация фрейма данных передатчиком. */
	frame_t bch_frame_in = transmitter->transmit_frame(transmitter);
	if (bch_frame_in == NULL) goto L_Stop;

	frame_display(bch_frame_in);

	/* Запись сгенерированного кодового слова БЧХ-кода в промежуточный буфер. */
	bch_frame_fifo = frame_fifo_put(bch_frame_fifo, bch_frame_in);

	/* Кодирование фрейма данных БЧХ-кодером и получение кодового слова БЧХ-кода. */
	codeword_t bch_codeword_in = bch_encoder->encode(bch_encoder, bch_frame_in);
	codeword_display(bch_codeword_in);

	/* Передача кодового слова сверточного кода по каналу. */
	codeword_t bch_codeword_out = channel_bs->transfer(channel_bs, bch_codeword_in);

	/* Декодирование кодового слова БЧХ-декодером и получение фрейма данных. */
	frame_t bch_frame_out = bch_decoder->decode(bch_decoder, bch_codeword_out, bch_codeword_in);
	frame_display(bch_frame_out);

	/* Извлечение из промежуточного буфера фрейма данных. */
	bch_frame_in = frame_fifo_get(bch_frame_fifo);

	/* Прием фрейма данных и анализ его корректности. */
	receiver->receive_frame(receiver, bch_frame_out, bch_frame_in);

	goto L_Loop;

	/************************************************************************/
	/* Останов программных компонентов.                                     */
	/************************************************************************/
L_Stop:
	transmitter->stop(transmitter);
	receiver->stop(receiver);
	bch_encoder->stop(bch_encoder);
	bch_decoder->stop(bch_decoder);
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
		0,
		ber);
	put_file_data(data_out, file_path, out_file_postfix);

	//print_data(data_in, data_size);
	//print_data(data_out, data_size);

	/************************************************************************/
	/* Уничтожение программных компонентов.                                 */
	/************************************************************************/
	transmitter_destroy(transmitter);
	receiver_destroy(receiver);
	bch_encoder_destroy(bch_encoder);
	bch_decoder_destroy(bch_decoder);
	channel_bs_destroy(channel_bs);
	frame_fifo_destroy(bch_frame_fifo);
	codeword_fifo_destroy(bch_codeword_fifo);
#ifdef _DEBUG	
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}

int kernel_run_bch_eraq_ch_mode
(
	int galois_field_degree,
	int bch_code_length,
	int error_correction,
	int decoded_seq_buf_size_frames,
	int random_error_quantity,
	int erase_error_quantity,
	char* out_file_postfix,
	char* input_file_name,
	int gui_progress
)
{
	FILE* flog = stdout;
	log(flog, "Current mode: use of BCH codec only via REQ+EEQ parametrized channel\n");
	log(flog, "Simulation kernel settings:\n");
	log(flog, "Galois field degree: %d\n",							galois_field_degree);
	log(flog, "BCH code length: %d\n",								bch_code_length);
	log(flog, "Error correction property: %d\n",					error_correction);
	log(flog, "Random errors quantity: %d\n",						random_error_quantity);
	log(flog, "Erase errors quantity: %d\n",						erase_error_quantity);
	log(flog, "Output file postfix: %s\n",							out_file_postfix);
	log(flog, "Input file name: %s\n",								input_file_name);

	/************************************************************************/
	/* Параметры тестового окружения.                                       */
	/************************************************************************/
	char* file_path = input_file_name;
	int* data_in = get_file_data(file_path);
	int data_size = get_file_size(file_path) * CHAR_BIT;
	//print_data(data_in, data_size);
	int* data_out = NULL;

	/************************************************************************/
	/* Параметры БЧХ-кодера                                                 */
	/************************************************************************/
	//int galois_field_degree = 4;
	//int bch_code_length = 15;
	//int error_correction = 3;

	/************************************************************************/
	/* Параметры ДСК-канала                                                 */
	/************************************************************************/
	//int random_error_quantity = 5;
	//int erase_error_quantity = 2;

	/************************************************************************/
	/* Создание программных компонентов.                                    */
	/************************************************************************/
	bch_encoder_t bch_encoder = bch_encoder_create(flog, galois_field_degree, bch_code_length, error_correction);
	bch_decoder_t bch_decoder = bch_decoder_create(ERRORS_ERASE_CORRECTION_MODE, flog, galois_field_degree, bch_code_length, error_correction);
	bch_decoder_set_erase_errors_q(bch_decoder, erase_error_quantity);

	/* Получение длины фрейма данных, который может кодировать БЧХ-кодер. */
	int bch_frame_size = bch_encoder_get_frame_size(bch_encoder);
	int bch_codeword_size = bch_encoder_get_codeword_size(bch_encoder);

	/************************************************************************/
	/* Создание программных компонентов.                                    */
	/************************************************************************/
	transmitter_t transmitter = transmitter_create(flog, bch_frame_size, data_in, data_size);
	receiver_t receiver = receiver_create(flog, bch_frame_size, data_size);

	/************************************************************************/
	/* Создание программных компонентов.                                    */
	/************************************************************************/
	channel_be_t channel_be = channel_be_create_q(flog, random_error_quantity, erase_error_quantity);

	/************************************************************************/
	/* Параметры промежуточных буферов                                      */
	/************************************************************************/
	int bch_frame_fifo_size = decoded_seq_buf_size_frames + 1;
	int bch_codeword_fifo_size = decoded_seq_buf_size_frames + 1;

	/************************************************************************/
	/* Создание промежуточных буферов.                                      */
	/************************************************************************/
	frame_fifo_t bch_frame_fifo = frame_fifo_create(bch_frame_fifo_size, bch_frame_size);
	codeword_fifo_t bch_codeword_fifo = codeword_fifo_create(bch_codeword_fifo_size, bch_codeword_size);

	/************************************************************************/
	/* Запуск программных компонентов.                                      */
	/************************************************************************/
	log(flog, "Simulation started\n");
	transmitter->start(transmitter);
	receiver->start(receiver);
	bch_encoder->start(bch_encoder);
	bch_decoder->start(bch_decoder);
	channel_be->start(channel_be);

	/************************************************************************/
	/* Запуск цикла моделирования.                                          */
	/************************************************************************/
	int sent_frames = 0;
	int frames_to_sent = data_size/bch_frame_size;
	int send_bits_all = data_size+decoded_seq_buf_size_frames*bch_frame_size;
L_Loop:
	/*if (!(sent_bits_cnt % percent_precision) && !gui_progress)
		printf("\rProgress: %.0f%%", ((float)sent_bits_cnt/
		(float)send_bits_all) * 100);
	else if (!(sent_bits_cnt % percent_precision))
		printf("Progress: %.0f%%\n", ((float)sent_bits_cnt/
		(float)send_bits_all) * 100);*/
	loadbar(sent_frames++, frames_to_sent, 50);

	/* Генерация фрейма данных передатчиком. */
	frame_t bch_frame_in = transmitter->transmit_frame(transmitter);
	if (bch_frame_in == NULL) goto L_Stop;

	frame_display(bch_frame_in);

	/* Запись сгенерированного кодового слова БЧХ-кода в промежуточный буфер. */
	bch_frame_fifo = frame_fifo_put(bch_frame_fifo, bch_frame_in);

	/* Кодирование фрейма данных БЧХ-кодером и получение кодового слова БЧХ-кода. */
	codeword_t bch_codeword_in = bch_encoder->encode(bch_encoder, bch_frame_in);
	codeword_display(bch_codeword_in);

	/* Передача кодового слова сверточного кода по каналу. */
	codeword_t bch_codeword_out = channel_be->transfer(channel_be, bch_codeword_in);

	/* Декодирование кодового слова БЧХ-декодером и получение фрейма данных. */
	frame_t bch_frame_out = bch_decoder->decode(bch_decoder, bch_codeword_out, bch_codeword_in);
	frame_display(bch_frame_out);

	/* Извлечение из промежуточного буфера фрейма данных. */
	bch_frame_in = frame_fifo_get(bch_frame_fifo);

	/* Прием фрейма данных и анализ его корректности. */
	receiver->receive_frame(receiver, bch_frame_out, bch_frame_in);

	goto L_Loop;

	/************************************************************************/
	/* Останов программных компонентов.                                     */
	/************************************************************************/
L_Stop:
	transmitter->stop(transmitter);
	receiver->stop(receiver);
	bch_encoder->stop(bch_encoder);
	bch_decoder->stop(bch_decoder);
	channel_be->stop(channel_be);
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
		0,
		0);
	put_file_data(data_out, file_path, out_file_postfix);

	//print_data(data_in, data_size);
	//print_data(data_out, data_size);

	/************************************************************************/
	/* Уничтожение программных компонентов.                                 */
	/************************************************************************/
	transmitter_destroy(transmitter);
	receiver_destroy(receiver);
	bch_encoder_destroy(bch_encoder);
	bch_decoder_destroy(bch_decoder);
	channel_be_destroy(channel_be);
	frame_fifo_destroy(bch_frame_fifo);
	codeword_fifo_destroy(bch_codeword_fifo);
#ifdef _DEBUG	
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}