/*********************************************************/
// Copyright (C) 2026 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/
#pragma once

#include <stdio.h>
#include <string.h>

#define CLEAR_FORMAT "\033[0m"
#if 0
	#define BOLD_RED "\033[1;37;41m"
	#define BOLD_BLUE "\033[1;37;44m"
	#define BOLD_GREEN "\033[1;30;42m"
	#define BOLD_PINK "\033[1;37;45m"
	#define BOLD_CYAN "\033[1;30;46m"
	#define BOLD_WHITE "\033[1m"
	#define BOLD_YELLOW "\033[1;30;43m"
#else
	#define BOLD_RED "\033[1;31m"
	#define BOLD_BLUE "\033[1;34m"
	#define BOLD_GREEN "\033[1;32m"
	#define BOLD_PINK "\033[1;35m"
	#define BOLD_CYAN "\033[1;36m"
	#define BOLD_BLACK_ON_CYAN "\033[1;30;46m"
	#define BOLD_WHITE "\033[1m"
	#define BOLD_YELLOW "\033[1;33m"
	#define BOLD_BLACK_ON_YELLOW "\033[1;30;43m"
#endif

//From state.h
extern int PLAIN_TEXT;
extern int COMPRESS_REPORT;
#define P_GENERIC(fd,format,x,color)\
	if(!PLAIN_TEXT) fprintf(fd,color); \
	fprintf(fd,format,x);\
	if(!PLAIN_TEXT) fprintf(fd,CLEAR_FORMAT);


#define P_WARNING(fd,format,name) P_GENERIC(fd,format,name,BOLD_RED)
#define P_COUNTERS(fd,format,name) P_GENERIC(fd,format,name,BOLD_BLACK_ON_YELLOW)
//#define P_COUNTERS(fd,format,name) P_GENERIC(fd,format,name,BOLD_YELLOW)
#define P_NAME(fd,format,name) P_GENERIC(fd,format,name,BOLD_BLACK_ON_CYAN)
//#define P_NAME(fd,format,name) P_GENERIC(fd,format,name,BOLD_CYAN)
#define P_NUMBER(fd,format,x) P_GENERIC(fd,format,x,BOLD_BLUE)
#define P_VL(fd,format,x) P_GENERIC(fd,format,x,BOLD_GREEN)
#define P_PERCENTAGE(fd,format,x) P_GENERIC(fd,format,x,BOLD_PINK)

#define P_NUMBER2(fd, format, x) P_GENERIC(fd,format,x,BOLD_GREEN) 
#define P_NUMBER3(fd, format, x) P_GENERIC(fd,format,x,BOLD_PINK)
#define P_NUMBER4(fd, format, x) P_GENERIC(fd,format,x,BOLD_YELLOW)
#define P_NUMBER5(fd, format, x) P_GENERIC(fd,format,x,BOLD_RED)

struct indent_control_t{
	char buffer[1024];
	int offset;
	short offsets[128]; //offsets point to where each nest symbol is
	int nesting;
	int clear_previous;
	int prev_nest;
	int spaces;
};
typedef struct indent_control_t indent_control_t;

//Exported variables
extern indent_control_t ic;

void reset_indent(void);
char * indent_add(int is_last);
char * indent_sub(int howmany, int is_last);
char * indent_none(int is_last);
void indent(FILE * fd, int level, int is_last);
