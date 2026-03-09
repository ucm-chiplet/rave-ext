/*********************************************************/
// Copyright (C) 2025 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/
#pragma once

void vcpu_insn_exec(unsigned int cpu_index, void *udata);

void region_trace(unsigned int cpu_index, int event, int value);

void vcpu_rave_event_string(unsigned int cpu_index, void * insn_opcode_void);
void vcpu_rave_value_string(unsigned int cpu_index, void * insn_opcode_void);
void vcpu_rave_event_and_value(unsigned int cpu_index, void * insn_opcode_void);
void vcpu_rave_name_event_value(unsigned int cpu_index, void* insn_opcode_void);

void vcpu_rave_begin_region(unsigned int cpu_index, void * insn_opcode_void);
void vcpu_rave_end_region(unsigned int cpu_index, void * insn_opcode_void);

void vcpu_parallel_end(unsigned int cpu_index, void * udata);
void vcpu_parallel_begin(unsigned int cpu_index, void* insn_opcode_void);
void vcpu_parallel_barrier(unsigned int cpu_index, void * udata);

void vcpu_restart_trace(unsigned int cpu_index, void *udata);

void vcpu_enable_regions(unsigned int cpu_index, void *udata);
void vcpu_disable_regions(unsigned int cpu_index, void *udata);

void vcpu_enable_trace(unsigned int cpu_index, void *udata);
void vcpu_disable_trace(unsigned int cpu_index, void *udata);
