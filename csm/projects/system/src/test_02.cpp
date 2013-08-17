//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: test_02.cpp
// Author: Pavel Morozkin
// Date: August 17th 2013
// Revised: August 17th 2013
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
#include "codec_cnv.h"
#include "test_utilities.h"

#include <stdlib.h>

int test_02()
{
	FILE* flog = stdout;

	/************************************************************************/
	/* Параметры тестового окружения.                                       */
	/************************************************************************/
	//char* file_path = "..\\test_files\\img_01.gif";
	char* file_path = "test_files\\text_01.txt";
	int* data_in = get_file_data(file_path);
	int data_size = get_file_size(file_path) * 8;
	//print_data(data_in, data_size);
	int* data_out = NULL;

	/************************************************************************/
	/* Параметры БЧХ-кодера                                                 */
	/************************************************************************/
	int galois_field_degree = 4;
	int bch_code_length = 15;
	int error_correction = 3;

	/************************************************************************/
	/* Параметры промежуточных буферов                                      */
	/************************************************************************/
	int bch_frame_fifo_size = 100;
	int bch_codeword_fifo_size = 100;

	int skip_q_cnt = 0;

	/************************************************************************/
	/* Создание программных компонентов.                                    */
	/************************************************************************/
	bch_encoder_t bch_encoder = bch_encoder_create(flog, galois_field_degree, bch_code_length, error_correction);
	bch_decoder_t bch_decoder = bch_decoder_create(flog, galois_field_degree, bch_code_length, error_correction);

	/* Получение длины фрейма данных, который может кодировать БЧХ-кодер. */
	int bch_frame_size = bch_encoder_get_frame_size(bch_encoder);
	int bch_codeword_size = bch_encoder_get_codeword_size(bch_encoder);

	/************************************************************************/
	/* Создание программных компонентов.                                    */
	/************************************************************************/
	transmitter_t transmitter = transmitter_create(flog, bch_frame_size, data_in, data_size);
	receiver_t receiver = receiver_create(flog, bch_frame_size, data_size);

	/************************************************************************/
	/* Параметры сверточного кодера.                                        */
	/************************************************************************/
	int regs_q = 2; /* ! do not change */
	int codeword_length = bch_code_length * regs_q;
	int decoded_seq_buf_size_frames = 1;
	int decoded_seq_buf_size = bch_code_length * decoded_seq_buf_size_frames + 1;
	int skip_q = decoded_seq_buf_size_frames;

	/************************************************************************/
	/* Создание программных компонентов.                                    */
	/************************************************************************/
	cnv_encoder_t cnv_encoder = cnv_encoder_create(flog, regs_q, codeword_length);
	cnv_decoder_t cnv_decoder = cnv_decoder_create(flog, regs_q, decoded_seq_buf_size);

	/************************************************************************/
	/* Создание промежуточных буферов.                                      */
	/************************************************************************/
	frame_fifo_t bch_frame_fifo = frame_fifo_create(bch_frame_fifo_size, bch_frame_size);
	codeword_fifo_t bch_codeword_fifo = codeword_fifo_create(bch_codeword_fifo_size, bch_codeword_size);

	/************************************************************************/
	/* Запуск программных компонентов.                                      */
	/************************************************************************/
	transmitter->start(transmitter);
	receiver->start(receiver);

	bch_encoder->start(bch_encoder);
	bch_decoder->start(bch_decoder);

	cnv_encoder->start(cnv_encoder);
	cnv_decoder->start(cnv_decoder);

	/************************************************************************/
	/* Запуск цикла моделирования.                                          */
	/************************************************************************/
L_Loop:
	/* Генерация фрейма данных передатчиком. */
	frame_t bch_frame_in = transmitter->transmit_frame(transmitter);
	if (bch_frame_in == NULL) goto L_Dummy_Generation;

	frame_display(bch_frame_in);

	/* Запись сгенерированного фрейма данных в промежуточный буфер. */
	bch_frame_fifo = frame_fifo_put(bch_frame_fifo, bch_frame_in);

	/* Кодирование фрейма данных БЧХ-кодером и получение кодового слова БЧХ-кода. */
	codeword_t bch_codeword_in = bch_encoder->encode(bch_encoder, bch_frame_in);
	codeword_display(bch_codeword_in);

	/* Преобразование кодового слова БЧХ-кода к фрейму данных для сверточного кодера. */
	frame_t cnv_frame_in = convert_codeword_to_frame(bch_codeword_in);
	frame_display(cnv_frame_in);

	/* Кодирование фрейма данных сверточным кодером и получение кодового слова сверточного кода.*/
	codeword_t cnv_codeword_in = cnv_encoder->encode(cnv_encoder, cnv_frame_in);
	goto L_Skip_Dummy_Generation;

L_Dummy_Generation:
	bch_codeword_in = codeword_create(bch_encoder->code_length);
	cnv_codeword_in = codeword_create(cnv_encoder->codeword_length);

L_Skip_Dummy_Generation:
	/* Запись сгенерированного кодового слова БЧХ-кода в промежуточный буфер. */
	bch_codeword_fifo = codeword_fifo_put(bch_codeword_fifo, bch_codeword_in);
	codeword_destroy(bch_codeword_in); 
	codeword_display(cnv_codeword_in);

	/* Эмуляция передачи кодового слова сверточного кода по каналу. */
	codeword_t cnv_codeword_out = codeword_create(cnv_codeword_in->xsize);
	cnv_codeword_out = codeword_cpy(cnv_codeword_out, cnv_codeword_in);
	codeword_display(cnv_codeword_out);	

	/* Декодирование кодового слова сверточного кода и получение фрейма данных. */
	frame_t cnv_frame_out = cnv_decoder->decode(cnv_decoder, cnv_codeword_out, cnv_codeword_in);
	frame_display(cnv_frame_out);

	/* Преобразование декодированного фрейма данных кодовому слову для БЧХ-декодирования. */
	codeword_t bch_codeword_out = convert_frame_to_codeword(cnv_frame_out);
	codeword_display(bch_codeword_out);	
	frame_destroy(cnv_frame_out);

	/* Проверка счетчика пропусков. */
	if(skip_q_cnt != skip_q) { 
		skip_q_cnt++; 
		codeword_destroy(bch_codeword_out); 
		goto L_Loop;
	}

	/* Извлечение из промежуточного буфера кодового слова БЧХ-кода. */
	bch_codeword_in = codeword_fifo_get(bch_codeword_fifo);
	codeword_display(bch_codeword_in);

	/* Декодирование кодового слова БЧХ-декодером и получение фрейма данных. */
	frame_t bch_frame_out = bch_decoder->decode(bch_decoder, bch_codeword_out, bch_codeword_in);
	frame_display(bch_frame_out);

	/* Извлечение из промежуточного буфера фрейма данных. */
	bch_frame_in = frame_fifo_get(bch_frame_fifo);
	frame_display(bch_frame_in);

	/* Прием фрейма данных и анализ его корректности. */
	receiver->receive_frame(receiver, bch_frame_out, bch_frame_in);

	/* Проверка наличия фреймов в буфере. */
	bch_frame_in = frame_fifo_get(bch_frame_fifo);
	if(bch_frame_in == NULL) goto L_Stop;
	else 
	{
		bch_frame_fifo = frame_fifo_put(bch_frame_fifo, bch_frame_in);
		frame_destroy(bch_frame_in);
		goto L_Loop;
	}

	/************************************************************************/
	/* Останов программных компонентов.                                     */
	/************************************************************************/
L_Stop:
	transmitter->stop(transmitter);
	receiver->stop(receiver);

	bch_encoder->stop(bch_encoder);
	bch_decoder->stop(bch_decoder);

	cnv_encoder->stop(cnv_encoder);
	cnv_decoder->stop(cnv_decoder);

	/************************************************************************/
	/* Проверка корректности принятых данных.                               */
	/************************************************************************/
	data_out = receiver_get_received_data(receiver);

	if (cmp_data(data_in, data_out, data_size))
		printf("test: OK\n");
	else
		printf("test: FAIL\n");

	free(data_in);

	put_file_data(data_out, file_path, "transferred");

	//print_data(data_in, data_size);
	//print_data(data_out, data_size);

	/************************************************************************/
	/* Уничтожение программных компонентов.                                 */
	/************************************************************************/
	transmitter_destroy(transmitter);
	receiver_destroy(receiver);

	bch_encoder_destroy(bch_encoder);
	bch_decoder_destroy(bch_decoder);

	cnv_encoder_destroy(cnv_encoder);
	cnv_decoder_destroy(cnv_decoder);

	frame_fifo_destroy(bch_frame_fifo);
	codeword_fifo_destroy(bch_codeword_fifo);

	return 0;
}