//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: test_01.cpp
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
#include "common.h"
#include "test_engine.h"

#include <stdlib.h>
#include <string.h>

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

int test_01()
{
	FILE* log = stdout;
	int frame_size = 10;
	int data_size = (int)1e6;
	int* data_in = (int*)calloc(data_size, sizeof(int));
	int* data_out = NULL;

	int times = 100;
	int i = 0;

	transmitter_t transmitter = transmitter_create(log, frame_size, data_in, data_size);
	receiver_t receiver = receiver_create(log, frame_size, data_size);

	transmitter->start(transmitter);
	receiver->start(receiver);

L_Loop:
	frame_t frame_in = transmitter->transmit_frame(transmitter);
	if (frame_in == NULL) goto L_Stop;
	
	frame_display(frame_in);

	/* channel emulation */
	frame_t frame_out = frame_create(frame_size);
	frame_out = frame_cpy(frame_out, frame_in);
	frame_display(frame_out);

	receiver->receive_frame(receiver, frame_out, frame_in);
	goto L_Loop;

L_Stop:
	transmitter->stop(transmitter);
	receiver->stop(receiver);

	data_out = receiver_get_received_data(receiver);

	if (cmp_data(data_in, data_out, data_size))
		printf("test: OK\n");
	else
		printf("test: FAIL\n");

	//print_data(data_in, data_size);
	//print_data(data_out, data_size);

	free(data_in);

	receiver_destroy(receiver);
	transmitter_destroy(transmitter);
	return 0;
}