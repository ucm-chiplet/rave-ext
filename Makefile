BUILD_DIR ?= $(shell pwd)/build
RVV ?= 1_0

ifeq ($(LLVM_DIR),)
  ifeq ($(RVV), 0_7_1)
    LLVM_DIR ?= $(BUILD_DIR)/llvm-cross/llvm-EPI-0.7-development-toolchain-cross
  else
    LLVM_DIR ?= $(BUILD_DIR)/llvm-cross/llvm-EPI-development-toolchain-cross
  endif
  LLVM_DEP=LLVM
endif


TARGETS_RV=QEMU ELFUTILS SYSROOT LLVM RAVE PARALLEL GDB validation_rvv
TARGETS_X86=DYNAMORIO validation_x86

COMMAND?=all


ifeq ($(ISA),x86)
TARGETS := $(TARGETS_X86) 
else ifeq ($(ISA),arm)
else
TARGETS := $(TARGETS_RV) 
endif

.PHONY: $(TARGETS)
all: $(TARGETS) 

QEMU: ; 
	make -f makefiles/make_QEMU.mk $(COMMAND) BUILD_DIR=$(BUILD_DIR) RVV=$(RVV)

ELFUTILS: ;
	make -f makefiles/make_ELFUTILS.mk $(COMMAND) BUILD_DIR=$(BUILD_DIR) 

SYSROOT: ;
	make -f makefiles/make_SYSROOT.mk $(COMMAND) BUILD_DIR=$(BUILD_DIR)

LLVM: ; 
	make -f makefiles/make_LLVM.mk $(COMMAND) BUILD_DIR=$(BUILD_DIR) RVV=$(RVV)

RAVE: QEMU ELFUTILS $(LLVM_DEP) SYSROOT
	make -f makefiles/make_RAVE.mk $(COMMAND) BUILD_DIR=$(BUILD_DIR) RVV=$(RVV) LLVM_DIR=$(LLVM_DIR)

PARALLEL: LLVM
	make -f makefiles/make_PARALLEL.mk $(COMMAND) BUILD_DIR=$(BUILD_DIR) LLVM_DIR=$(LLVM_DIR)

GDB: ;
	make -f makefiles/make_GDB.mk $(COMMAND) BUILD_DIR=$(BUILD_DIR) RVV=$(RVV)

validation_rvv: RAVE 
	make -f makefiles/make_validation_rvv.mk $(COMMAND) BUILD_DIR=$(BUILD_DIR) RVV=$(RVV) LLVM_DIR=$(LLVM_DIR)


DYNAMORIO: ;
	make -f makefiles/make_DYNAMORIO.mk $(COMMAND) BUILD_DIR=$(BUILD_DIR) ISA=$(ISA)
XAVE: DYNAMORIO 
	make -f makefiles/make_XAVE.mk $(COMMAND) BUILD_DIR=$(BUILD_DIR)
validation_x86: XAVE
	make -f makefiles/make_validation_x86.mk $(COMMAND) BUILD_DIR=$(BUILD_DIR)
