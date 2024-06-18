# QEMU-SDV



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


# Running RISC-V binaries

Two scripts are provided to run your RISC-V binaries, `./build/EPI/bin/rave` and `./build/EPI-0.7/bin/rave` (use them accordingly to the RVV specification used in your code).

These scripts are also controled by the following environment variables:
 - **QEMU_PRINT_SCALAR**: If set to \"1\", adds tracing information for each scalar instruction (trace gets a lot bigger). Otherwise, scalar instructions are treated as bursts. (default: 0)
 - **QEMU_PRINT_LOGFILE**: If set to \"1\", a logfile is generated with all the executed instructions. (default: 0)
 - **QEMU_LOGFILE_NAME**: Sets the name of the generated logfile (default: qemulog.log). Additionally, automatically sets QEMU_PRINT_LOGFILE to 1
 - **QEMU_VLEN**: Sets the maximum available vector-length in bits (default: 16384)
 - **QEMU_PRINT_PRV**: If set to \"1\", a paraver trace is generated with all the executed instructions. (default: 0). 
 - **QEMU_PRV_NAME**: Sets the name of the generated paraver trace (default: qemutrace). Additionally, automatically sets QEMU_PRINT_PRV to 1
 - **QEMU_PRINT_REPORT**: If set to "1", the tracer will print a hardware counter summary for each executed code region. (default: 0)
 - **QEMU_PRINT_CSV**: If set to "1", the tracer will print a CSV with the hardware counter summary for each executed code region. (default: 0)

For example, you can run your RVV0_7 code while generating a report and a prv trace like this:

```bash
QEMU_PRV_NAME=test_trace QEMU_PRINT_REPORT=1 ./build/EPI-0.7/bin/rave ./yourcode.x arguments
```

You can find Paraver configuration files in the `CFGs` folder. We recommend using value 1000 to instrument your code, so all CFGs work as expected.

In folder `test` you can find a code example instrumented with QEMU.
