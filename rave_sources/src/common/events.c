/*********************************************************/
// Copyright (C) 2025 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/

#include "events.h"
#include "utils.h"
#include "threading.h"
#include "rave2prv.h"
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

void rave_event_and_value(uint32_t insn_opcode, thread_state_t * state){
	if (!TRACE_ENABLED) return;

	int src1 = (insn_opcode>>15)&0x1F;
	int src2 = (insn_opcode>>20)&0x1F;


	int qemu_trace_event = qemu_get_xreg(state,src1);
	int qemu_trace_value = qemu_get_xreg(state,src2);

	set_lock(write_lock);
	//rave_eventandcounters(qemu_trace_event, qemu_trace_value, cpu_index, &cpus_state[cpu_index].accum_counters);
	if (PRINT_PRV){
		/*
			 if (parallel_region.master_thread == -1){ //Not in a parallel region -> Propagate event to all threads
			 for(int cpu_id = 0; cpu_id < alloc_threads; ++cpu_id){
			 trace_row(FD_PRV,mpi_rank, cpu_id, SCALAR_ROW, timestamp);
			 trace_event_value(FD_PRV,qemu_trace_event,qemu_trace_value);
			 if (!TRACE_SCALAR) trace_event_value(FD_PRV,event_instruction, 1000);
			 trace_row(FD_PRV,mpi_rank, cpu_id, VECTOR_ROW, timestamp);
			 trace_event_value(FD_PRV,qemu_trace_event,qemu_trace_value);
			 }
			 }else{ //In a parallel region -> Event is local to this thread
			 */
		uint64_t thread_timestamp = state->timestamp;
		trace_row(FD_PRV,mpi_rank, state->cpu_index, SCALAR_ROW, thread_timestamp);
		trace_event_value(FD_PRV,qemu_trace_event,qemu_trace_value);
		if (!TRACE_SCALAR) trace_event_value(FD_PRV,event_instruction, PRV_SCALAR*!MUSA);
		trace_row(FD_PRV,mpi_rank, state->cpu_index, VECTOR_ROW, thread_timestamp);
		trace_event_value(FD_PRV,qemu_trace_event,qemu_trace_value);
		//}
	}
	release_lock(write_lock);
}

void rave_name_event_value(uint32_t insn_opcode, thread_state_t * state){
	int src1 = (insn_opcode>>15)&0x1F;
	int src2 = (insn_opcode>>20)&0x1F;
	state->rave_event_number = qemu_get_xreg(state,src1);
	state->rave_value_number = qemu_get_xreg(state,src2);
}

void rave_event_string(uint32_t insn_opcode, thread_state_t * state){
	char data[128];
	rave_read_string(state->cpu_index, insn_opcode, data, 128);
	add_event(state->rave_event_number,data); 
}
void rave_value_string(uint32_t insn_opcode, thread_state_t * state){
	char data[128];
	rave_read_string(state->cpu_index, insn_opcode, data, 128);
	add_value_to_event(state->rave_event_number,state->rave_value_number,data); 
}
