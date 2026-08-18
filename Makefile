BUILD_DIR ?= $(shell pwd)/build

ISA ?= RVV-1_0

#Backwards compatibility with manual RVV setting (now we use ISA)
ifeq ($(RVV),1_0)
  ISA = RVV-1_0
else ifeq ($(RVV),0_7_1)
  ISA = RVV-0_7_1
endif

ifeq ($(ISA),RVV-1_0)
  RVV=1_0
  LLVM_DIR ?= $(BUILD_DIR)/llvm-cross/llvm-EPI-development-toolchain-cross
  LLVM_DEP=LLVM
else ifeq ($(ISA),RVV-0_7_1)
  RVV=0_7_1
  LLVM_DIR ?= $(BUILD_DIR)/llvm-cross/llvm-EPI-0.7-development-toolchain-cross
  LLVM_DEP=LLVM
else
  $(error Unknown ISA: "$(ISA)")
endif

TARGETS_RV=QEMU ELFUTILS SYSROOT LLVM RAVE PARALLEL GDB validation_rvv

COMMAND?=all

TARGETS := $(TARGETS_RV) 

.PHONY: $(TARGETS)
all: $(TARGETS) 

help: ;
	@echo Compile all dependencies using \"make ISA=[ISA_NAME]\" where ISA_NAME can be:
	@echo "  - RVV-1_0"
	@echo "  - RVV-0_7_1"

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
