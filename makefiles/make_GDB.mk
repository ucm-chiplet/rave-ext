ifndef BUILD_DIR
	$(error Need to define vairable BUILD_DIR)
endif
ifndef RVV
  $(error Need to define variable RVV)
endif

GDB_REPO=https://github.com/riscvarchive/riscv-binutils-gdb
ifeq ($(RVV),0_7_1)
  GDB_REPO += -b rvv-0.7.1
endif
GDB_BUILD_DIR=$(BUILD_DIR)/gdb/gdb-rvv-$(RVV)
GDB_BUILD_TARGET=$(GDB_BUILD_DIR)/bin/riscv64-linux-gnu-gdb
GDB_DIR=downloads/riscv-binutils-gdb-$(RVV)
JOBS=12

all: $(GDB_BUILD_TARGET)

$(GDB_DIR): ;
	git clone $(GDB_REPO) $@

$(GDB_DIR)/Makefile: | $(GDB_DIR)
	cd $(GDB_DIR) && ./configure --prefix=$(GDB_BUILD_DIR) --target=riscv64-linux-gnu

$(GDB_BUILD_TARGET): $(GDB_DIR)/Makefile | $(GDB_DIR)
	cd $(GDB_DIR) && make -j $(JOBS) && make install 

clean:
	rm -rf $(GDB_DIR) $(GDB_BUILD_DIR)
