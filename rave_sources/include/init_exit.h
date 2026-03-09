/*********************************************************/
// Copyright (C) 2025 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/
#pragma once
#include <qemu-plugin.h>

void plugin_exit(qemu_plugin_id_t id, void *p);

QEMU_PLUGIN_EXPORT int qemu_plugin_install(qemu_plugin_id_t id,
		const qemu_info_t *info, int argc,
		char **argv);
