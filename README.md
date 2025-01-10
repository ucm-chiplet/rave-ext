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

```bash
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

### 3. Control de RAVE execution and generate reports

Additionally, you can control RAVE's execution and output using these environment variables: 

 - **RAVE_PRINT_SCALAR**: If set to \"1\", adds tracing information for each scalar instruction (trace gets a lot bigger). Otherwise, scalar instructions are treated as bursts. (default: 0)
 - **RAVE_PRINT_LOGFILE**: If set to \"1\", a logfile is generated with all the executed instructions. (default: 0).
 - **RAVE_LOGFILE_NAME**: Sets the name of the generated logfile (default: qemulog.log). Additionally, automatically sets RAVE_PRINT_LOGFILE to 1.
 - **RAVE_VLEN**: Sets the maximum available vector-length in bits (default: 16384).
 - **RAVE_PRINT_PRV**: If set to \"1\", a paraver trace is generated with all the executed instructions. (default: 0). 
 - **RAVE_PRV_NAME**: Sets the name of the generated paraver trace (default: qemutrace). Additionally, automatically sets RAVE_PRINT_PRV to 1.
 - **RAVE_PRINT_REPORT**: If set to "1", the tracer will print a hardware counter summary for each executed code region. (default: 0).
 - **RAVE_PRINT_CSV**: If set to "1", the tracer will print a CSV with the hardware counter summary for each executed code region. (default: 0).
 - **RAVE_CSV_NAME**: Sets the name of the generated csv trace (default: qemu_summary.csv). Additionally, automatically sets RAVE_PRINT_CSV to 1. 
 - **RAVE_SYSROOT**: Sets the path to a user-specified RISC-V sysroot."

For example, generate a RAVE report like this:

```bash
RAVE_PRINT_REPORT=1 rave ./example-c.x
...
Region #2: Event 1000 (code_region), Value 2 (ini_B), Rank 0, Thread 0
	Moved bytes (Total): 20606
		Moved bytes (scalar): 22 (0.11 %)
		Moved bytes (vector): 20584 (99.89 %)
	tot_instr: 90
		scalar_instr: 57 (63.33 %)
		vsetvl_instr: 11 (12.22 %)
		vector_instr: 22 (24.44 %)
			SEW 8 vector_instr: 0 (0.00 %)
			SEW 16 vector_instr: 0 (0.00 %)
			SEW 32 vector_instr: 0 (0.00 %)
			SEW 64 vector_instr: 22 (100.00 %)
				avg_VL: 233.91 elements
				Arith: 0 (0.00 %)
					FP: 0 (0.00 %)
					INT: 0 (0.00 %)
				Mem: 11 (50.00 %)
					unit: 11 (100.00 %)
					strided: 0 (0.00 %)
					indexed: 0 (0.00 %)
				Mask: 0 (0.00 %)
				Other: 11 (50.00 %)
...
```

You can generate this report in a CSV format using the "RAVE_CSV_NAME" or "RAVE_PRINT_CSV" environment variables.

### 4. Generate Paraver traces with RAVE

RAVE also allows to generate Paraver traces (that can be visualized in Paraver):

```bash
RAVE_PRV_NAME=example_trace rave ./example-c.x
```

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

#### Sequential trace example

In a sequential trace, you will find two rows of data: One for the scalar instructions, and another for the vector ones.


### 5. Run OMP programs with RAVE

Althought RAVE does not specifically provide parallelization metrics, you can run OMP binaries with it.

In folder `test/axpy/` you will find a parallelized and vectorized Axpy code. Compile and run it like this:


```bash
cd ../axpy
make axpy-omp
OMP_NUM_THREADS=4 rave ./axpy-omp.x
```

In an OMP RAVE report, regions of code will be identified by their execution thread. If within a region new threads are spawned, the counters from the threads are added to the original region's thread.

In an OMP paraver trace, you will find as many touples of scalar-vector rows as OMP threads.

### 6. Run MPI programs with RAVE

In a similar fashion, you can run MPI binaries with RAVE.

In the same folder (`test/axpy/`) you can compile Axpy with MPI parallelization using the mpicc cross-compiler (which should be in your PATH if you correctly sourced the environment script), and run it with RAVE using you system's mpirun. 

```bash
make axpy-mpi
mpirun -np 4 rave ./axpy-mpi.x
```


It is advised not to generate console reports with MPI binaries, as all processes will print data simultaneously. Instead, generating a CSV file will create one per each process.

In an MPI trace you will find groups of rows per each process. Each has a touples of scalar-vector rows per each OMP thread of the specific process. 

## RAVE API (code instrumentation)

You can instrument your code using the RAVE API. Althought you can use RAVE without instrumentation, we recommend taking a look on the API's functions defined in the `interfaces/rave_user_events.h` header:

 - **rave_name_event(int x, char \* name)**: Assigns `name` to event `x`.
 - **rave_name_value(int x, int y, char \* name)**: Assigns `name` to value `y` of event `x`.
 - **rave_restart_trace()**: Erase all traced metrics and counters up to this point, and start tracing again.
 - **rave_start_trace()**: After this call, record metrics and generate trace files.
 - **rave_stop_trace()**: After this call, do not record metrics or generate trace files.
 - **rave_event_and_value(x,y)**: Add a tuple of event=`x` and value=`y` to the trace, used to separate code regions

You can then use this API in your code and compilations.  

#### Using the RAVE API on C or C++

On your source file:

```c
#include "rave_user_events.h"
```

```c
#include "rave_user_events.h"
int main(){

	int N = 256*10 + 13;
	double A[N];

	rave_name_event(1000,"code_region");
	rave_name_value(1000,0,"End");
	rave_name_value(1000,1,"ini_A");

	rave_restart_trace();

	rave_stop_trace()
	/* ... Non-traced code ... */
	rave_start_trace()

	rave_event_and_value(1000,1)
	for(int i=0; i<N; ++i)	A[i] = i;
	rave_event_and_value(1000,0)
	//...
```

On your Makefile:

```bash
$(CC)/$(CXX) -I${RAVE_INCLUDE} ...
```

#### Using the RAVE API on Fortran

On your source file:

```fortran
program example
  use rave_user_events
  integer, parameter :: N = 256*10+13 
  real(8) :: A(N)
  integer :: i

  call rave_name_event(1000,"code_region")
  call rave_name_value(1000,0_8,"End")
  call rave_name_value(1000,1_8,"ini_A")

	call rave_restart_trace()
	call rave_stop_trace()
	! ... Non-traced code ... 
	call rave_start_trace()

  call rave_event_and_value(1000,1_8)
  do i = 1, N
      A(i) = i-1
  end do
  call rave_event_and_value(1000,0_8)
	! ...
```

On your Makefile:

```bash
$(FC) -I${RAVE_INCLUDE} ${RAVE_INCLUDE}/rave_user_events_f.o ...
```

#### Using the RAVE API on Python

```python
import rave_user_events
def main():
    rave_user_events.rave_name_event(1000,"code_region")
    rave_user_events.rave_name_value(1000,0,"End")
    rave_user_events.rave_name_value(1000,1,"ini_A")

    rave_user_events.rave_restart_trace()
		rave_user_events.rave_stop_trace()
		# ... Non-traced code ... 
		rave_user_events.rave_start_trace()

    N = 256*10 + 13
    A = [0]*N

    rave_user_events.rave_event_and_value(1000,1)
    for i in range(N):
        A[i] = i
    rave_user_events.rave_event_and_value(1000,0)
		# ...
```

## Using RAVE: TUTORIAL (Relevant Material!)

Here you can download [the slides presented at the RISC-V Techincal Session](https://ssh.hca.bsc.es/epi/ftp/RAVE/RAVE_RISC-V_Technical_Session.pdf) on the 10th of October, 2024.
In the presentation, [this demo code](https://ssh.hca.bsc.es/epi/ftp/RAVE/SDV_Tutorial_rave.tar.gz) is used to showcase RAVE's potential. You can see the [video tutorial in this link](https://www.youtube.com/watch?v=7eUnhmvcDtY)

## Citing RAVE

```
@article{vizcaino2024rave,
  title={{RAVE: RISC-V Analyzer of Vector Executions, a QEMU tracing plugin}},
  author={{Vizcaino, Pablo and Mantovani, Filippo and Labarta, Jesus and Ferrer, Roger}},
  journal={{arXiv preprint arXiv:2409.13639}},
  year={2024}
}
```
