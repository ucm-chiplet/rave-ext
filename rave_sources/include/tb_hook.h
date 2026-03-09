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

char is_rave_api(uint32_t insn_opcode, struct qemu_plugin_insn * insn);
void vcpu_tb_trans(qemu_plugin_id_t id, struct qemu_plugin_tb *tb);
