/*********************************************************/
// Copyright (C) 2026 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/
#pragma once

#include <stdint.h>
#include <qemu-plugin.h>


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

void plugin_exit(qemu_plugin_id_t id, void *p);

QEMU_PLUGIN_EXPORT int qemu_plugin_install(qemu_plugin_id_t id,
		const qemu_info_t *info, int argc,
		char **argv);

char is_rave_api(uint32_t insn_opcode, struct qemu_plugin_insn * insn);
void vcpu_tb_trans(qemu_plugin_id_t id, struct qemu_plugin_tb *tb);
