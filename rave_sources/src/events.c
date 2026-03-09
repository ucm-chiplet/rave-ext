/*********************************************************/
// Copyright (C) 2025 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/

#include "events.h"
#include "utils.h"
#include <stdlib.h>

event_info * first_event_info = NULL;
event_info * last_event_info = NULL;

event_info * find_event(int id){
	event_info * curr = first_event_info;
	while (curr!=NULL){
		if (curr->ID == id) return curr;
		curr = curr->next;
	}
	return NULL;
}

value_info * find_value(event_info * event, int val){
	value_info * curr = event->values;
	while (curr!=NULL){
		if (curr->ID == val) return curr;
		curr = curr->next;
	}
	return NULL;
}

value_info *  add_new_value(event_info * event, int val, char * name){
	value_info * new_values = (value_info*)malloc(sizeof(value_info));
	new_values -> next = NULL;
	new_values -> ID = val;
	my_strcpy(new_values->name, name);

	//Add it to the value stack
	if (event->last_value == NULL){
		event->values = new_values;
		event->last_value = new_values;
		return new_values;
	}
	event->last_value->next = new_values;
	event->last_value = new_values;
	return new_values;
}

void add_value_to_event(int id, int val, char * name){
	event_info * event = find_event(id);
	if (event == NULL){
		//printf("Event %d not found\n",id);
	 	return;
	}

	//If value already exists, just update its name
	value_info * value = find_value(event, val);
	if (value != NULL){
		free(value->name);
		//printf("%d %d found, update name from %s to %s\n", id,val,value->name, name);
		my_strcpy(value->name, name);
		return;
	}

	//value not found: create it
	//printf("%d %d not found, named it %s\n",id,val,name);
	add_new_value(event,val,name);
}

event_info * add_event(int id, const char *name){

	event_info * event = find_event(id);
	if (event != NULL){
		free(event->name);
		my_strcpy(event->name, name);
		return event;
	}
	//event not found: create it
	event_info * new_event = (event_info*)malloc(sizeof(event_info));
	new_event -> next = NULL;
	new_event -> ID = id;
	new_event -> values = NULL;
	new_event -> last_value = NULL;
	my_strcpy(new_event->name, name);

	if (first_event_info==NULL) first_event_info = new_event;
	if (last_event_info!=NULL) last_event_info->next = new_event;
	last_event_info = new_event;

	return new_event;
}
