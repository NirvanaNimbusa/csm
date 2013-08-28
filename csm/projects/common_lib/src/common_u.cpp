//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: common_u.cpp
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
#include "common.h"

#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <iomanip>

static int log_string_length = 50;

static void strip_new_line(char *s)
{
	char *p = s;
	int n;
	while (*s)
	{
		n = strcspn(s, "\n");
		strncpy(p, s, n);
		p += n;
		s += n + strspn(s+n, "\n");
	}
	*p = 0;
}

int log (FILE* stream, const char* format, ...)
{
	va_list args;
	va_start(args, format);
/*
	char *uri_field = "mail:username@example.com";
	char username[64];
	sscanf(uri_field, "mail:%63[^@]", username);
	sscanf(uri_field, "%*[^:]:%63[^@]", username);
*/
	char message[128] = {0};
	char data[128];
	if(strchr(const_cast<char*>(format), '%'))
	{
		printf("- ");
		if(sscanf(const_cast<char*>(format), "%127[^%]", message) != EOF)
		{
			fprintf(stream, message);
			for (unsigned int i = 0; i < log_string_length - strlen(message); i++)
				fprintf(stream, ".");
			fprintf(stream, " ");
			strcpy(data, strchr(const_cast<char*>(format), '%'));
			strip_new_line(data);
			vfprintf(stream, data, args);
			printf("\n");
		}
	}
	else
		vfprintf(stream, format, args);
	va_end(args);
	return 0;
}

int log2 (FILE* stream, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	//vprintf(format, args);
	//vfprintf(stream, format, args);
	va_end(args);
	return 0;
}

int log3 (FILE* stream, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	//vprintf(format, args);
	//vfprintf(stream, format, args);
	va_end(args);
	return 0;
}

int xexit(const char* format, ...)
{
	log(stderr, "xexit report: an error occured!\n");
	log(stderr, "- reason: ");
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	log(stderr, "exiting...");
	exit(EXIT_FAILURE);
	return 0;
}

frame_t convert_codeword_to_frame(codeword_t codeword)
{
	int frame_size = codeword->xsize;
	frame_t frame = frame_create(frame_size);

	/* assume that bch_codeword_in->xsize = 15 */
	for (int i = 0; i < codeword->xsize; i++)
		frame->xframe[i] = codeword->xcodeword[i];

	return frame;
}

codeword_t convert_frame_to_codeword(frame_t frame)
{
	int codeword_size = frame->xsize;
	codeword_t codeword = codeword_create(codeword_size);

	/* copy bch_codeword_size symbols */
	for (int i = 0; i < codeword->xsize; i++)
		codeword->xcodeword[i] = frame->xframe[i];

	return codeword;
}

void printf_d(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	//vprintf(format, args);
	va_end(args);
}

char* extend_out_file_postfix(
	char* out_file_postfix,
	int galois_field_degree,
	int bch_code_length,
	int error_correction,
	int decoded_seq_buf_size_frames,
	double ber
	)
{
	const int extend_out_file_postfix_size = 128;
	char* extend_out_file_postfix = (char*)calloc(sizeof(char), extend_out_file_postfix_size);
	if(extend_out_file_postfix)
	{
		char buffer [33];

		strcat(extend_out_file_postfix, out_file_postfix);

		strcat(extend_out_file_postfix, "_g_");
		_itoa (galois_field_degree, buffer, 10);
		strcat(extend_out_file_postfix, buffer);

		strcat(extend_out_file_postfix, "_l_");
		_itoa (bch_code_length, buffer, 10);
		strcat(extend_out_file_postfix, buffer);

		strcat(extend_out_file_postfix, "_e_");
		_itoa (error_correction, buffer, 10);
		strcat(extend_out_file_postfix, buffer);

		strcat(extend_out_file_postfix, "_d_");
		_itoa (decoded_seq_buf_size_frames, buffer, 10);
		strcat(extend_out_file_postfix, buffer);

		strcat(extend_out_file_postfix, "_b_");
		sprintf(buffer, "%.6f", ber);
		strcat(extend_out_file_postfix, buffer);

		return extend_out_file_postfix;
	}
	return NULL;
}

void loadbar(unsigned int x, unsigned int n, unsigned int w)
{
	if ( (x != n) && (x % (n/100) != 0) ) return;

	float ratio    =  x/(float)n;
	int c =  (int)(ratio * w);

	std::cout << "[";
	for ( int x=0; x<c; x++) std::cout << "=";
	for (unsigned int x=c; x<w; x++) std::cout << " ";
	std::cout << "] " << std::setw(3) << (int)(ratio*100) << "%\r" << std::flush;
}

int invert_bit(int bit)
{
	return bit ? 0 : 1;
}