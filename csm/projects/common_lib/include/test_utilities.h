//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: test_utilities.h
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
#ifndef _TEST_UTILITIES_
#define _TEST_UTILITIES_

int cmp_data(int* f_data, int* s_data, int data_size);
void print_data(int* data_in, int data_size);
int* get_file_data(char* file_path);
void put_file_data(int* data, char* file_path, char* file_postfix);
int get_file_size(char* img_path);
char* byte2bin(int i);

#endif