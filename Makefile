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


TARGETS=QEMU ELFUTILS SYSROOT LLVM RAVE PARALLEL GDB validation
.PHONY: $(TARGETS)

COMMAND?=all

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

validation: RAVE 
	make -f makefiles/make_validation.mk $(COMMAND) BUILD_DIR=$(BUILD_DIR) RVV=$(RVV) LLVM_DIR=$(LLVM_DIR)
