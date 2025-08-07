[[_TOC_]]

# RAVE

The RISC-V Analyzer of Vector Executions (RAVE) is a QEMU plugin that simulates the EPAC VEC tile, allowing users to run on binaries compiled for the rvv1.0 and rvv0.7 RISC-V extensions.


## Installation

We strongly encourage to follow the following installation steps in the specific order they appear

### 1. Clone repo

First, clone the repo (recommended **without** recursing subumodules, as they can be quite heavy):

```bash
git clone https://repo.hca.bsc.es/gitlab/pvizcaino/rave.git
```

### 2. Install QEMU

Then, install the QEMU emulator with either the `0_7` or `1_0` flag to select the RVV specification (installations go to separate folders):

```bash
./install_qemu.sh [0_7 / 1_0]
```

### 3. Install LLVM-based RISC-V cross-compiler (x86)

Then, download and install the LLVM-based cross-compiler using the following script:

```bash
./install_compiler.sh [0_7 / 1_0]
```

### 4. Install the RAVE plugin

Then install ELFUTILS and RAVE
```bash
./install_elfutils.sh
./install_rave.sh [0_7 / 1_0]
```

### 5. Install a RISC-V sysroot for emulated binaries

Donwload and install it with the following script:

```bash
./install_sysroot.sh 
```

### 6. Install the Parallel support for RAVE (OMP and MPI)

```bash
./install_parallel.sh
```

### 7. Install GDB for RAVE:

```bash
./install_gdb.sh [0_7 / 1_0]
```

## Testing

In order to compile and vectorize RISC-V binaries, and emulate them with RAVE, we recommend loading the enviroment script (after having followed the installation steps):

For RVV 0.7.1:
```bash
RVV=0_7 source environment.sh
```

For RVV 1.0:
```bash
RVV=1_0 source environment.sh
```


### 1. Compile the example codes

In the `test/examples` folder you can find the same example code in C, C++, Fortran, and Python, instrumented using the [RAVE API](#rave-api)

Go into the `test/examples` folder and run make (you can look into the `Makefile` file to see the compilation flags to vectorize the code and include the RAVE API):

```bash
RVV=1_0 source environment.sh
cd test/examples/
make
```

### 2. Emulate and profile binaries with RAVE

You can emulate the execution of the example binary with RAVE like this:

```bash
rave ./example-c.x
```


### 3. Control the RAVE execution with environment variables

Additionally, you can control RAVE's execution and output using these environment variables: 

Trace extra information:  
 - **RAVE_TRACE_SCALAR**: If set to \"1\", adds tracing information for each scalar instruction (trace gets a lot bigger). Otherwise, scalar instructions are treated as bursts. (default: 0)
 - **RAVE_TRACE_ADDR**: If set to \"1\", adds tracing information of the base address of vector loads/stores. (default: 0)

Control RAVE internals:
 - **RAVE_VLEN**: Sets the maximum available vector-length in bits (default: 16384).
 - **RAVE_SYSROOT**: Sets the path to a user-specified RISC-V sysroot."
 - **RAVE_CUSTOM_EXTENSIONS**: Appends RISC-V extensions to the QEMU cpu (e.g. \"zicbom=true,zicboz=true,zicbop=true,zicond=true\" to emulate the bananapif3 boards). 

Control logfile generation:
 - **RAVE_PRINT_LOGFILE**: If set to \"1\", a logfile is generated with all the executed instructions. (default: 0).
 - **RAVE_LOGFILE_NAME**: Sets the name of the generated logfile (default: qemulog.log). Additionally, automatically sets RAVE_LOGFILE to 1.

Control PRV generation:
 - **RAVE_PRINT_PRV**: If set to \"1\", a paraver trace is generated with all the executed instructions. (default: 0). 
 - **RAVE_PRV_NAME**: Sets the name of the generated paraver trace (default: qemutrace). Additionally, automatically sets RAVE_PRV to 1.
 - **RAVE_REGION_EVENT**: Set paraver event where the first nesting level of regions will be mapped. (default: 1000) 

Control report / profile / csv generation:
 - **RAVE_PLAIN_TEXT**: Print the report/profile without colours or highlighted text. (default: 0).
 - **RAVE_ACCUM_REGIONS**: If set to "1", a region that appears twice will be aggregated/accumulated. (default: 0).

 - **RAVE_PRINT_REPORT**: If set to "1", the tracer will print to stdout a hardware counter summary for each executed code region. (default: 0).
 - **RAVE_REPORT_NAME**: Redirects the report to the provided file name. Additionally, automatically sets RAVE_PRINT_REPORT to 1. 

 - **RAVE_PRINT_PROFILE**: If set to "1", the tracer will print to stdout a profiling of the most time consuming regions. (default: 0).
 - **RAVE_PROFILE_NAME**: Redirects the profile to the provided file name. Additionally, automatically sets RAVE_PRINT_PROFILE to 1. 

 - **RAVE_PRINT_CSV**: If set to "1", the tracer will print a CSV with the hardware counter summary for each executed code region. (default: 0).
 - **RAVE_CSV_NAME**: Sets the name of the generated csv trace (default: qemu_summary.csv). Additionally, automatically sets RAVE_PRINT_CSV to 1. 


## 4. Generate reports and profiles.

### RAVE API (code instrumentation)

You can instrument your code using the RAVE API. Althought you can use RAVE without instrumentation, we recommend taking a look on the API's functions defined in the `interfaces/rave_user_events.h` header. You can instrument the code into regions using:

 - **rave_begin_region(char \* name)**: Starts a region with the given name. If another region was open, increases the nesting level.
 - **rave_end_region(char \* name)**: Ends a region with that given name.
 - **rave_restart_trace()**: Erase all traced metrics and counters up to this point, and start tracing again.
 - **rave_start_trace()**: After this call, record metrics, regions, and generate trace files.
 - **rave_stop_trace()**: After this call, do not record metrics, regions, or generate trace files.

You can also add event and value tupples to your paraver traces using these calls:
 - **rave_name_event(int x, char \* name)**: Assigns `name` to event `x`.
 - **rave_name_value(int x, int y, char \* name)**: Assigns `name` to value `y` of event `x`.
 - **rave_event_and_value(x,y)**: Add a tuple of event=`x` and value=`y` to the trace, used to separate code regions

You can then use this API in your code and compilations.  

##### Using the RAVE API on C or C++

On your source file:

```c
#include "rave_user_events.h"
int main(){

	int N = 256*10 + 13;
	double A[N];

	rave_restart_trace();

	rave_stop_trace()
	/* ... Non-traced code ... */
	rave_start_trace()

	rave_begin_region("ini_A") 
	for(int i=0; i<N; ++i)	A[i] = i;
	rave_end_region("ini_A")
	//...
```

On your Makefile:

```bash
$(CC)/$(CXX) -I${RAVE_INCLUDE} ...
```

##### Using the RAVE API on Fortran

On your source file:

```fortran
program example
  use rave_user_events
  integer, parameter :: N = 256*10+13 
  real(8) :: A(N)
  integer :: i


	call rave_restart_trace()
	call rave_stop_trace()
	! ... Non-traced code ... 
	call rave_start_trace()

  call rave_begin_region("ini_A")
  do i = 1, N
      A(i) = i-1
  end do
  call rave_end_region("ini_A")
	! ...
```

On your Makefile:

```bash
$(FC) -I${RAVE_INCLUDE} ${RAVE_INCLUDE}/rave_user_events_f.o ...
```

##### Using the RAVE API on Python

```python
import rave_user_events
def main():

    rave_user_events.rave_restart_trace()
		rave_user_events.rave_stop_trace()
		# ... Non-traced code ... 
		rave_user_events.rave_start_trace()

    N = 256*10 + 13
    A = [0]*N

  	rave_user_events.rave_begin_region("ini_A")
    for i in range(N):
        A[i] = i
  	rave_user_events.rave_end_region("ini_A")
		# ...
```

### Examples of reports and profiles

For example, generate a RAVE report like this:

```bash
RAVE_PRINT_REPORT=1 rave ./example-c.x
...
    │    ├─ Region #2: ini_A [Nesting: 2] (Rank: 0, Thread: 0)
    │    │    └─ Counters:
    │    │         ├─ Moved bytes: 5148
    │    │         │    ├─ Scalar: 5148 (100.00 %)
    │    │         │    └─ Vector: 0
    │    │         └─ Instructions: 12872
    │    │              ├─ Scalar instr: 12872 (100.00 %)
    │    │              ├─ Vsetvl instr: 0
    │    │              └─ Vector instr: 0
    │    │                   ├─ SEW 8 vector instr: 0
    │    │                   ├─ SEW 16 vector instr: 0
    │    │                   ├─ SEW 32 vector instr: 0
    │    │                   └─ SEW 64 vector instr: 0
    │    └─ Region #3: ini_B [Nesting: 2] (Rank: 0, Thread: 0)
    │         └─ Counters:
    │              ├─ Moved bytes: 20606
    │              │    ├─ Scalar: 22 (0.11 %)
    │              │    └─ Vector: 20584 (99.89 %)
    │              └─ Instructions: 91
    │                   ├─ Scalar instr: 58 (63.74 %)
    │                   ├─ Vsetvl instr: 11 (12.09 %)
    │                   └─ Vector instr: 22 (24.18 %)
    │                        ├─ SEW 8 vector instr: 0
    │                        ├─ SEW 16 vector instr: 0
    │                        ├─ SEW 32 vector instr: 0
    │                        └─ SEW 64 vector instr: 22 (100.00 %)  [avg VL: 233.91 elements]
    │                             ├─ Arith: 0
    │                             ├─ Memory: 11 (50.00 %)  [avg VL: 233.91 elements]
    │                             │    ├─ unit: 11 (100.00 %)
    │                             │    ├─ strided: 0
    │                             │    └─ indexed: 0
    │                             ├─ Mask: 0
    │                             └─ Other: 11 (50.00 %)  [avg VL: 233.91 elements]
...
```

If you want to redirect the report to a file, do:
```bash
RAVE_REPORT_NAME=myfile rave ./example-c.x
```

You can generate this report in a CSV format using the "RAVE_CSV_NAME" or "RAVE_PRINT_CSV" environment variables.

you can also generate a profile of your application:

```bash
RAVE_PRINT_PROFILE=1 rave ./example-c.x
------------------- PROFILE --------------------
 └─ GLOBAL_REGION .... executions: 1, total instr: 15064 (100.00 % of total, 100.00 % of parent)
   ├─ initialization . executions: 1, total instr: 13122 (87.11 % of total, 87.11 % of parent)
   │ ├─ ini_A ........ executions: 1, total instr: 12872 (85.45 % of total, 98.09 % of parent)
   │ └─ ini_B ........ executions: 1, total instr: 91 (0.60 % of total, 0.69 % of parent)
   └─ compute ........ executions: 1, total instr: 334 (2.22 % of total, 2.22 % of parent)
     ├─ arith_vec .... executions: 1, total instr: 176 (1.17 % of total, 52.69 % of parent)
     └─ if_vec ....... executions: 1, total instr: 142 (0.94 % of total, 42.51 % of parent)
------------------------------------------------
```

Remember to use `RAVE_ACCUM_REGIONS` if you want to accumulate regions that have the same name.

### 5. Generate Paraver traces with RAVE

RAVE also allows to generate Paraver traces (that can be visualized in Paraver):

```bash
RAVE_PRV_NAME=example_trace rave ./example-c.x
```

You can use the environment variable `RAVE_REGION_EVENT` to dictate which Paraver event corresponds to the lower-nesting region. By default, it's event 1000.

This will generate a triplet of files called `example_prv.prv`, `example_prv.pcf`, and `example_prv.row`.
You can copy these files back to your computer and open the trace in Paraver. If you don't have Paraver, you can download it from [the bsc tools webpage](https://tools.bsc.es/downloads)
```bash
wxparaver example_trace.prv
```

You can find Paraver configuration files in the `PRV_CFGs` folder:

 - **Instruction_timeline.cfg:** Opens a sequence of simulated instructions, with one row for scalar and another for vector instructions. Scalar instructions are not individually separated unless "RAVE_PRINT_SCALAR" was set.

 - **Bytes_per_vector.cfg:** Opens the sequence/evolution of the vector length (in Bytes) per instruction.

 - **table_instruction_type_count.cfg:** Opens a table with the number of simulated instructions per each type.

 - **table_average_bytes_per_instruction_type.cfg:** Opens a table with the average vector length (in Bytes) per each instruction type.

 - **table_class_mix.cfg:** Opens a table with the mix of vector instructions by their type (Artihmetic, Memory, ...)

 - **PC.cfg:** Opens a view with the program counter of the instructions (scalar instructions are not separated unless "RAVE_PRINT_SCALAR" was set).

In the subfolder `CFGs/per_phase_cfgs` you will find configuration files that can be used when your code has been instrumented with event 1000:
 - **event_1000_code_region.cfg:** Opens the sequence of instrumented code regions, with their width equal to the number of simulated instructions.

 - **table_vector_mix_per_phase.cfg:** Opens two tables, one with the absolute number of scalar and vector instructions per phase, and another with their relative numbers (what we usually call Vector Mix).

 - **table_instruction_type_count_per_phase.cfg:** Opens a table that contains the number of simulated instructions per each type. The table can be configured (3D.Plane) to select which instrumented code phase is analyzed.

 - **table_class_mix_per_phase.cfg:** Opens a table with the mix of vector instructions by their type (Artihmetic, Memory, ...) per each code phase.

 - **table_average_vl_per_phase.cfg:** Opens a table with the average vector length of each instrumented code phase.

 - **table_average_vl_per_instruction_per_phase.cfg:** Opens a table that contains the averaged vector length per each simulated instruction type. The table can be configured (3D.Plane) to select which instrumented code phase is analyzed.


### 6. Using RAVE with GDB

If your emulated code has e.g. a SEGFAULT and you want to debug it, you can do so using the `rave_gdb` utility (:warning: Remember to compile your code with "-g"!):

```bash
rave_gdb ./main.x 

----------------------------------------------------------
Creating RAVE emulation with port 4210212
GDB running
Running GDB with target remote localhost:4210212
Write "c" or "continue" in GDB to start your program
----------------------------------------------------------
/apps/x86/rave/rave-2502260926/gdb/gdb-rvv-0_7/share/gdb/python/gdb/command/prompt.py:48: SyntaxWarning: "is not" with a literal. Did you mean "!="?
  if self.value is not '':
/apps/x86/rave/rave-2502260926/gdb/gdb-rvv-0_7/share/gdb/python/gdb/command/prompt.py:60: SyntaxWarning: "is not" with a literal. Did you mean "!="?
  if self.value is not '':
GNU gdb (GDB) 8.2.50.20190202-git
Copyright (C) 2019 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "--host=x86_64-pc-linux-gnu --target=riscv64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ././main.x...
Remote debugging using localhost:4210212
warning: remote target does not support file transfer, attempting to access files from local filesystem.
warning: Unable to find dynamic linker breakpoint function.
GDB will be unable to debug shared library initializers
and track explicitly loaded dynamic code.
0x00000040008125e0 in ?? ()
Reading symbols from /apps/x86/rave/rave-2502260926/qemu-rave/RVV-0_7_1/bin/../../../sysroot/lib/ld-linux-riscv64-lp64d.so.1...
(No debugging symbols found in /apps/x86/rave/rave-2502260926/qemu-rave/RVV-0_7_1/bin/../../../sysroot/lib/ld-linux-riscv64-lp64d.so.1)
(gdb) 
```

From this point, use "continue" or "c" to start the program, and "bt" to print the back-trace:
```bash
(gdb) c
Continuing.
warning: Could not load shared library symbols for /apps/x86/rave/rave-2502260926/qemu-rave/RVV-0_7_1/bin/../../../parallel/ompt.so.
Do you need "set solib-search-path" or "set sysroot"?

Program received signal SIGSEGV, Segmentation fault.
_wordcopy_fwd_aligned (dstp=<error reading variable: dwarf2_find_location_expression: Corrupted DWARF expression.>, srcp=<error reading variable: dwarf2_find_location
    _expression: Corrupted DWARF expression.>, len=<error reading variable: dwarf2_find_location_expression: Corrupted DWARF expression.>) at wordcopy.c:79
79	wordcopy.c: No such file or directory.
(gdb) bt
#0  _wordcopy_fwd_aligned (dstp=<error reading variable: dwarf2_find_location_expression: Corrupted DWARF expression.>, srcp=<error reading variable: dwarf2_find_location_expression: Corrupted DWARF expression.>, len=<error reading variable: dwarf2_find_location_expression: Corrupted DWARF expression.>) at wordcopy.c:79
#1  0x00000040008a7bde in __GI_memcpy (dstpp=<error reading variable: dwarf2_find_location_expression: Corrupted DWARF expression.>,
     srcpp=<error reading variable: dwarf2_find_location_expression: Corrupted DWARF expression.>, len=<error reading variable: dwarf2_find_location_expression: Corrupted DWARF expression.>) at memcpy.c:51
#2  0x00000000000105de in myfunc (A=0x132a0, B=0x26b30, N=10000) at main.c:4
#3  main (argc=<optimized out>, argv=<optimized out>) at main.c:13
```

In this case, the SEGFAULT occurs on `main.c:4`:
```c
1: void myfunc(double * A, double * B, int N){
2:
3:	for(int i=0; i<N; ++i){
4:		A[i] = B[1000000000+i];
5:	}
6: }
```


### 7. Run OMP programs with RAVE

Althought RAVE does not specifically provide parallelization metrics, you can run OMP binaries with it.

In folder `test/axpy/` you will find a parallelized and vectorized Axpy code. Compile and run it like this:


```bash
cd ../axpy
make axpy-omp
OMP_NUM_THREADS=4 rave ./axpy-omp.x
```

In an OMP RAVE report, regions of code will be identified by their execution thread. If within a region new threads are spawned, the counters from the threads are added to the original region's thread.

In an OMP paraver trace, you will find as many touples of scalar-vector rows as OMP threads.

### 8. Run MPI programs with RAVE

In a similar fashion, you can run MPI binaries with RAVE.

In the same folder (`test/axpy/`) you can compile Axpy with MPI parallelization using the mpicc cross-compiler (which should be in your PATH if you correctly sourced the environment script), and run it with RAVE using you system's mpirun. 

```bash
make axpy-mpi
mpirun -np 4 rave ./axpy-mpi.x
```


It is advised not to generate console reports with MPI binaries, as all processes will print data simultaneously. Instead, generating a CSV file will create one per each process.

In an MPI trace you will find groups of rows per each process. Each has a touples of scalar-vector rows per each OMP thread of the specific process. 


## Using GDB to debug RAVE

If your emulated code has e.g. a SEGFAULT and you want to debug it, you can do so using the `rave_gdb` utility (:warning: Remember to compile your code with "-g"!):

```bash
rave_gdb ./main.x 
----------------------------------------------------------
Creating RAVE emulation with port 4210212
GDB running
Running GDB with target remote localhost:4210212
Write "c" or "continue" in GDB to start your program
----------------------------------------------------------
/apps/x86/rave/rave-2502260926/gdb/gdb-rvv-0_7/share/gdb/python/gdb/command/prompt.py:48: SyntaxWarning: "is not" with a literal. Did you mean "!="?
  if self.value is not '':
/apps/x86/rave/rave-2502260926/gdb/gdb-rvv-0_7/share/gdb/python/gdb/command/prompt.py:60: SyntaxWarning: "is not" with a literal. Did you mean "!="?
  if self.value is not '':
GNU gdb (GDB) 8.2.50.20190202-git
Copyright (C) 2019 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "--host=x86_64-pc-linux-gnu --target=riscv64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ././main.x...
Remote debugging using localhost:4210212
warning: remote target does not support file transfer, attempting to access files from local filesystem.
warning: Unable to find dynamic linker breakpoint function.
GDB will be unable to debug shared library initializers
and track explicitly loaded dynamic code.
0x00000040008125e0 in ?? ()
Reading symbols from /apps/x86/rave/rave-2502260926/qemu-rave/RVV-0_7_1/bin/../../../sysroot/lib/ld-linux-riscv64-lp64d.so.1...
(No debugging symbols found in /apps/x86/rave/rave-2502260926/qemu-rave/RVV-0_7_1/bin/../../../sysroot/lib/ld-linux-riscv64-lp64d.so.1)
(gdb) 
```

From this point, use "continue" or "c" to start the program, and "bt" to print the back-trace:
```bash
(gdb) c
Continuing.
warning: Could not load shared library symbols for /apps/x86/rave/rave-2502260926/qemu-rave/RVV-0_7_1/bin/../../../parallel/ompt.so.
Do you need "set solib-search-path" or "set sysroot"?

Program received signal SIGSEGV, Segmentation fault.
_wordcopy_fwd_aligned (dstp=<error reading variable: dwarf2_find_location_expression: Corrupted DWARF expression.>, srcp=<error reading variable: dwarf2_find_location
    _expression: Corrupted DWARF expression.>, len=<error reading variable: dwarf2_find_location_expression: Corrupt
    ed DWARF expression.>) at wordcopy.c:79
79	wordcopy.c: No such file or directory.
(gdb) bt
#0  _wordcopy_fwd_aligned (dstp=<error reading variable: dwarf2_find_location_expression: Corrupted DWARF expression.>, srcp=<error reading variable: dwarf2_find_
    location_expression: Corrupted DWARF expression.>, len=<error reading variable: dwarf2_find_location_expression: Corrupt
    ed DWARF expression.>) at wordcopy.c:79
#1  0x00000040008a7bde in __GI_memcpy (dstpp=<error reading variable: dwarf2_find_location_expression: Corrupted DWARF expression.>,
     srcpp=<error reading variable: dwarf2_find_location_expression: Corr
    upted DWARF expression.>, len=<error reading variable: dwarf2_find_location_expression: Corrup
    ted DWARF expression.>) at memcpy.c:51
#2  0x00000000000105de in myfunc (A=0x132a0, B=0x26b30, N=10000) at main.c:4
#3  main (argc=<optimized out>, argv=<optimized out>) at main.c:13
```

In this case, the SEGFAULT occurs on `main.c:4`:
```c
1: void myfunc(double * A, double * B, int N){
2:
3:	for(int i=0; i<N; ++i){
4:		A[i] = B[1000000000+i];
5:	}
6: }
```

If the SEGFAULT does not happen within your program, contact `pablo.vizcaino@bsc.es`

## Using RAVE: TUTORIAL (Relevant Material!)

Here you can download [the slides presented at the RISC-V Techincal Session](https://ssh.hca.bsc.es/epi/ftp/RAVE/RAVE_RISC-V_Technical_Session.pdf) on the 10th of October, 2024.

In the presentation, [this demo code](https://ssh.hca.bsc.es/epi/ftp/RAVE/SDV_Tutorial_rave.tar.gz) is used to showcase RAVE's potential. You can see the [video tutorial in this link](https://www.youtube.com/watch?v=7eUnhmvcDtY)

Be aware that RAVE undergoes improvements and changes, so these resources might me outdated, specially the instrumentation part.

## Citing RAVE

```
@article{vizcaino2024rave,
  title={{RAVE: RISC-V Analyzer of Vector Executions, a QEMU tracing plugin}},
  author={{Vizcaino, Pablo and Mantovani, Filippo and Labarta, Jesus and Ferrer, Roger}},
  journal={{arXiv preprint arXiv:2409.13639}},
  year={2024}
}
```

## Contact us!

For any inquiry, contact `pablo.vizcaino@bsc.es`
