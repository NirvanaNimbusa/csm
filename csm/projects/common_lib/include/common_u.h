//*******************************************************************************
// Title: Communication System Modeler v.1.0
// File: common_u.h
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
#ifndef _COMMON_U_
#define _COMMON_U_

#include "frame_t.h"
#include "codeword_t.h"

#include <stdio.h>
#include <stdarg.h>

int log (FILE* stream, const char* format, ...);
int log2 (FILE* stream, const char* format, ...);
int log3 (FILE* stream, const char* format, ...);

int xexit(const char* format, ...);
frame_t convert_codeword_to_frame(codeword_t codeword);
codeword_t convert_frame_to_codeword(frame_t frame);
void printf_d(const char *format, ...);
char* extend_out_file_postfix(
	char* out_file_postfix,
	int galois_field_degree,
	int bch_code_length,
	int error_correction,
	int decoded_seq_buf_size_frames,
	double ber
	);

#endif