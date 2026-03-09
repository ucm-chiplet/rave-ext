/*********************************************************/
// Copyright (C) 2025 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/
#pragma once

#include <stdint.h>

struct value_info{
	struct value_info * next;
	char * name;
	int64_t ID;
};
typedef struct value_info value_info;

struct event_info{
	struct event_info * next;
	char * name; 
	int64_t ID;
	value_info * values;
	value_info * last_value;
};
typedef struct event_info event_info;

//Exported variables
extern event_info * first_event_info;

event_info * find_event(int id);
value_info * find_value(event_info * event, int val);
value_info *  add_new_value(event_info * event, int val, char * name);
void add_value_to_event(int id, int val, char * name);
event_info * add_event(int id, const char *name);

