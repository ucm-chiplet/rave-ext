[[_TOC_]]

# RAVE

The RISC-V Analyzer of Vector Executions (RAVE) is a QEMU plugin that simulates the EPAC VEC tile, allowing users to run on binaries compiled for the rvv1.0 and rvv0.7 RISC-V extensions.

# Installation

First, clone the repo (recommended **without** recursing subumodules, as they can be quite heavy):

```bash
git clone https://repo.hca.bsc.es/gitlab/pvizcaino/qemu-sdv.git
```

Then, install the QEMU emulator with either the `0_7` or `1_0` flag to select the RVV specification (installations go to separate folders):

```bash
./install_qemu.sh [0_7 / 1_0]
```

After that, install the QEMU tracing plugins using again the `0_7` or `1_0` flag:

```bash
./install_plugins.sh [0_7 / 1_0]
```

Finally, install the RISC-V toolchain to provide a sysroot to your QEMU Virtual Machine. This is independent of the RVV specification and should be installed just once, as it is quite time-consuming. **WARNING**: If you already have a RISC-V sysroot installed in your machine, you can edit the `./run_qemu_0_7.sh` and `./run_qemu_1_0.sh` files to change the sysroot

```bash
./install_toolchain.sh 
```

# Compiling RVV binaries

You may need a vectorizing compiler to generate RVV binaries.

The LLVM-based cross-compiler used on the EPI project is avaiable online, for [rvv1.0](https://ssh.hca.bsc.es/epi/ftp/LATEST_llvm-EPI-development-toolchain-cross_IS_2024-06-17-1541) and [rvv0.7](https://ssh.hca.bsc.es/epi/ftp/LATEST_llvm-EPI-0.7-release-toolchain-cross_IS_2022-10-10-1012).


# Running RISC-V binaries

Two scripts are provided to run your RISC-V binaries, `./build/EPI/bin/rave` and `./build/EPI-0.7/bin/rave` (use them accordingly to the RVV specification used in your code).

For example, you can run a rvv0.7 binary like this:
```bash
./build/EPI-0.7/bin/rave ./yourcode.x arguments
```

If your binary depends on dynamic libraries found in the environment variable `LD_LIBRARY_PATH`, you can either compile it statically or prepend your simulation with the library path:

```bash
LIBRARY_PATH=/path/to/lib:$LD_LIBRARY_PATH ./build/EPI-0.7/bin/rave ./yourcode.x arguments 
```

# Analyzing and Tracing RAVE simulations

Besides simulating the binary, RAVE can be used to instrument, trace, and analyze your code.

## Instrumenting code

You can include the header `include/rave_user_events.h` in your C code to add instrumentation, specifically these functions:

 - **rave_name_event(int x, char \* name)**: Assigns `name` to event `x`.
 - **rave_name_value(int x, int y, char \* name)**: Assigns `name` to value `y` of event `x`.
 - **rave_restart_trace()**: Erase all traced metrics and counters up to this point, and start tracing again.
 - **rave_start_trace()**: After this call, record metrics and generate trace files.
 - **rave_stop_trace()**: After this call, do not record metrics or generate trace files.
 - **rave_event_and_value(x,y)**: Add a tuple of event=`x` and value=`y` to the trace, used to separate code regions

This file can be included in your compilation after loading the rave module by using the `RAVE_INCLUDE` environment variable:

```bash
clang -O3 -mepi -I$(RAVE_INCLUDE) source.c -o source.x
```

We also provide an example code instrumented with rave on `./test/example.c`

You can compile it like this:

```bash
cd test
make example
```

## Controlling RAVE

You can control the RAVE simulation using the following environment variables:

 - **RAVE_PRINT_SCALAR**: If set to \"1\", adds tracing information for each scalar instruction (trace gets a lot bigger). Otherwise, scalar instructions are treated as bursts. (default: 0)
 - **RAVE_PRINT_LOGFILE**: If set to \"1\", a logfile is generated with all the executed instructions. (default: 0).
 - **RAVE_LOGFILE_NAME**: Sets the name of the generated logfile (default: qemulog.log). Additionally, automatically sets RAVE_PRINT_LOGFILE to 1.
 - **RAVE_VLEN**: Sets the maximum available vector-length in bits (default: 16384).
 - **RAVE_PRINT_PRV**: If set to \"1\", a paraver trace is generated with all the executed instructions. (default: 0). 
 - **RAVE_PRV_NAME**: Sets the name of the generated paraver trace (default: qemutrace). Additionally, automatically sets RAVE_PRINT_PRV to 1.
 - **RAVE_PRINT_REPORT**: If set to "1", the tracer will print a hardware counter summary for each executed code region. (default: 0).
 - **RAVE_PRINT_CSV**: If set to "1", the tracer will print a CSV with the hardware counter summary for each executed code region. (default: 0).
 - **RAVE_CSV_NAME**: Sets the name of the generated csv trace (default: qemu_summary.csv). Additionally, automatically sets RAVE_PRINT_CSV to 1. 

For example, you can run your RVV0_7 code while generating a report and a csv trace like this:

```bash
RAVE_CSV_NAME=example_csv RAVE_PRINT_REPORT=1 ./build/EPI-0.7/bin/rave ./test/example.x
```

Regions of code within calls to `rave_event_and_value(x,y)` report various instruction metrics:

```
...
Region #3: Event 1000 (code_region), Value 4 (arith_vec)
	Moved bytes (Total): 82358
		Moved bytes (scalar): 22 (0.03 %)
		Moved bytes (vector): 82336 (99.97 %)
	tot_instr: 191
		scalar_instr: 92 (48.17 %)
		vsetvl_instr: 11 (5.76 %)
		SEW 8 vector_instr: 0 (0.00 %)
		SEW 16 vector_instr: 0 (0.00 %)
		SEW 32 vector_instr: 0 (0.00 %)
		SEW 64 vector_instr: 88 (46.07 %)
			avg_VL: 233.91 elements
			Arith: 44 (50.00 %)
				FP: 44 (100.00 %)
				INT: 0 (0.00 %)
			Mem: 44 (50.00 %)
				unit: 44 (100.00 %)
				strided: 0 (0.00 %)
				indexed: 0 (0.00 %)
			Mask: 0 (0.00 %)
			Other: 0 (0.00 %)
...
```

## Generating Paraver traces

RAVE also allows to generate PRV traces that can be visualized in Paraver like this:

```bash
RAVE_PRV_NAME=example_prv ./build/EPI-0.7/bin/rave ./test/example.x
```

This will generate a triplet of files called `example_prv.prv`, `example_prv.pcf`, and `example_prv.row`.
You can copy these files back to your computer and open the trace in paraver:
```bash
wxparaver example_prv.prv
```



You can find Paraver configuration files in the `CFGs` folder:

 - **Instruction_timeline.cfg:** Opens a sequence of simulated instructions, with one row for scalar and another for vector instructions. Scalar instructions are not individually separated unless "RAVE_PRINT_SCALAR" was set.

 - **Bytes_per_vector.cfg:** Opens the sequence/evolution of the vector length (in Bytes) per instruction.

 - **table_instruction_type_count.cfg:** Opens a table with the number of simulated instructions per each type.

 - **table_average_bytes_per_instruction_type.cfg:** Opens a table with the average vector length (in Bytes) per each instruction type.

In the subfolder `/apps/x86/rave/share/CFGs/per_phase_cfgs` you will find configuration files that can be used when your code has been instrumented with event 1000:
 - **event_1000_code_region.cfg:** Opens the sequence of instrumented code regions, with their width equal to the number of simulated instructions.

 - **table_vector_mix_per_phase.cfg:** Opens two tables, one with the absolute number of scalar and vector instructions per phase, and another with their relative numbers (what we usually call Vector Mix).

 - **table_instruction_type_count_per_phase.cfg:** Opens a table that contains the number of simulated instructions per each type. The table can be configured (3D.Plane) to select which instrumented code phase is analyzed.

 - **table_average_vl_per_phase.cfg:** Opens a table with the average vector length of each instrumented code phase.

 - **table_average_vl_per_instruction_per_phase.cfg:** Opens a table that contains the averaged vector length per each simulated instruction type. The table can be configured (3D.Plane) to select which instrumented code phase is analyzed.
