//*******************************************************************************
// Title: Communication System Modeler v.1.1
// File: test_utilities.cpp
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
#include "common_u.h"
#include "test_utilities.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>

int cmp_data(int* f_data, int* s_data, int data_size)
{
	for (int i = 0; i < data_size; i++)
		if (f_data[i] != s_data[i])
			return 0;
	return 1;
}

void print_data(int* data_in, int data_size)
{
	for (int i = 0; i < data_size; )
	{
		printf("%d", data_in[i]);
		if (++i % 8 == 0)
			printf(" ");
		if (i % 64 == 0)
			printf("\n");
	}
	printf("\n");
}

int* get_file_data(char* file_path)
{
	FILE  *src_file = NULL;
	int file_size;
	int* data = NULL;
	int byte;

	src_file = fopen(file_path, "rb");
	if(src_file)
	{
		file_size = get_file_size(file_path);
		int size = file_size * CHAR_BIT;
		data = (int*)calloc(size, sizeof(int));
		if(data)
		{
			int byte_cnt = 0;
			for (int ii = 0; ii < file_size; ii++)
			{
				byte  =  fgetc(src_file);
				//printf("%d\n", byte);

				unsigned int i;
				i = 1 << (sizeof(char) * CHAR_BIT - 1);

				while (i > 0) 
				{
					if (byte & i)
						data[byte_cnt] = 1;
					else
						data[byte_cnt] = 0;
					byte_cnt++;
					i >>= 1;
				}
			}
			fclose(src_file);
			return data;
		}
	}
	return NULL;
}

void put_file_data(int* data, char* file_path,  char* file_postfix)
{
	FILE  *out_file = NULL;
	int file_size;
	int byte;
	char* byte_str = NULL;

 	char* file_name_full = (char*)calloc( strlen(file_path)+strlen(file_postfix) + 2, sizeof(char));
	if(file_name_full)
	{
		//strcat(file_name_full, file_path);
		//strcat(file_name_full, file_postfix);
		char* ext = strrchr(file_path, '.');

		/* make 'file_name' */
		int i = 0;
		for (; file_path[i] != *ext; i++)
			file_name_full[i] = file_path[i];

		/* make 'file_name.' */
		file_name_full[i] = '.';

		/* make 'file_name.postfix' */
		strcat(file_name_full, file_postfix);
		free(file_postfix);

		/* make 'file_name.ext' */
		strcat(file_name_full, ext);


		// replace '.' to '_', because
		// "! LaTeX Error: Unknown graphics extension: .transferred_g_4_l_15_e_3_d_31_b_0.0"
		int k = 0;
		for (;i < (int)strlen(file_name_full); i++)
		{
			if(file_name_full[i] == '.')
			{
				file_name_full[i] = '_';
				k++;
			}
			if (k == 2)
				break;
		}
		out_file = fopen(file_name_full, "wb");
	}

	if(file_name_full && out_file)
	{
		free(file_name_full);
		file_size = get_file_size(file_path);
		int data_size = file_size * CHAR_BIT;

		int data_cnt = 0;
		char* buffer = (char*)calloc(file_size, sizeof(char));
		if(buffer)
		{
			for (int ii = 0; ii < file_size; ii++)
			{
				int t = 7;
				byte = 0;
				for (int i = 0; i < CHAR_BIT; i++, data_cnt++, t--)
					byte += data[data_cnt] ? 1 << t : 0;

				buffer[ii] = byte;
				//printf("%d\n", byte);
			}

			fwrite (buffer, 1, file_size, out_file);
			free(buffer);
			fclose(out_file);
		}
	}
}

int get_file_size(char* file_path)
{
	FILE  *src_file;
	int file_size;

	src_file = fopen(file_path, "rb");
	if(src_file)
	{
		fseek(src_file, 0, SEEK_END); // seek to end of file
		file_size = ftell(src_file); // get current file pointer
		fclose(src_file);
		return file_size;
	}
	return NULL;
}

char * byte2bin(int n)
{
	int cnt = 0;
	char * str = (char*)malloc(sizeof(char) * CHAR_BIT);
	if(str)
	{
		unsigned int i;
		i = 1 << (sizeof(char) * CHAR_BIT - 1);

		while (i > 0) 
		{
			if (n & i)
				str[cnt] = '1';
			else
				str[cnt] = '0';
			cnt++;
			i >>= 1;
		}

		return str;
	}
	return NULL;
}