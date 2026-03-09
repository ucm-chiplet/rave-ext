ifndef BUILD_DIR
	$(error Need to define variable BUILD_DIR)
endif
ifndef RVV
	$(error Need to define variable RVV)
endif
ifndef LLVM_DIR
  $(error Need to provide variable LLVM_DIR)
endif

OBJ= \
formatting.o \
instr_data.o \
profiling.o \
rave2prv.o \
callbacks.o \
counters.o \
events.o \
init_exit.o \
regions.o \
threading.o \
utils.o \
state.o \
tb_hook.o 

CC=gcc
CFLAGS=-O3 -Wall -Werror -fPIC
CFLAGS+= `pkg-config --cflags glib-2.0`

LIBDIR=$(BUILD_DIR)/qemu-rave/RVV-$(RVV)/lib
BINDIR=$(BUILD_DIR)/qemu-rave/RVV-$(RVV)/bin
RAVE_DIR=rave_sources

ifeq ($(RVV),0_7_1) 
OBJ += 07_decode.o instr2prv_0_7.o write_pcf_0_7.o
QEMU_DIR = ./downloads/qemu-rvv-0_7_1
CFLAGS += -DRVV_07
OBJDIR=$(RAVE_DIR)/obj/rvv07
else
OBJ += instr2prv_1_0.o write_pcf_1_0.o
QEMU_DIR=./downloads/qemu-rvv-1_0
OBJDIR=$(RAVE_DIR)/obj/rvv10
#CFLAGS += -DQEMU_PLUGIN_VERSION=4
endif

OBJ:=$(addprefix $(OBJDIR)/, $(OBJ))

ELFUTILS_DIR=./build/elfutils
IFLAGS=-I$(RAVE_DIR)/include -I$(QEMU_DIR)/include/qemu -I$(ELFUTILS_DIR)/include

LDFLAGS=-L${ELFUTILS_DIR}/lib -lelf -ldw

.SECONDARY: $(OBJ)
.PHONY: wrappers interfaces 
all: $(LIBDIR)/librave.so wrappers interfaces 

#### LIBRAVE

$(OBJDIR): ;
	mkdir -p $@
$(LIBDIR): ;
	mkdir -p $@

$(OBJDIR)/%.o: $(RAVE_DIR)/src/%.c | $(OBJDIR)
	$(CC) $^ $(CFLAGS) $(IFLAGS) -c -o $@

$(LIBDIR)/librave.so: $(OBJ) | $(LIBDIR)
	$(CC) $^ -shared -fPIC -o $@ $(LDFLAGS)


#### WRAPPERS
$(BINDIR): ;
	mkdir -p $@
$(BINDIR)/rave: | $(BINDIR)
	cp $(RAVE_DIR)/rave-$(RVV).sh $@
$(BINDIR)/rave_gdb: | $(BINDIR)
	cp ./utils/rave_gdb $@
	sed -i "s/EXT/${RVV}/g" $@
wrappers: $(BINDIR)/rave $(BINDIR)/rave_gdb


### INTERFACES

CLANG=$(LLVM_DIR)/bin/clang
FLANG=$(LLVM_DIR)/bin/flang
INTERFACES_DIR=$(BUILD_DIR)/interfaces

$(INTERFACES_DIR): ;
	mkdir -p $@

$(INTERFACES_DIR)/rave_user_events_f.o: | $(INTERFACES_DIR)
	$(CLANG) $(RAVE_DIR)/interfaces/rave_fortran_c_func.c -c -o $(OBJDIR)/rave_fortran_c_func.o
	$(FLANG) $(RAVE_DIR)/interfaces/rave_user_events.f90 -c -o $(OBJDIR)/rave_user_events_tmp.o -J$(INTERFACES_DIR)
	$(CLANG) -r $(OBJDIR)/rave_fortran_c_func.o $(OBJDIR)/rave_user_events_tmp.o -o $@

$(INTERFACES_DIR)/rave_user_events.h: $(RAVE_DIR)/interfaces/rave_user_events.h | $(INTERFACES_DIR)
	cp $< $@

$(INTERFACES_DIR)/rave_user_events.so: $(RAVE_DIR)/interfaces/py_module.py | $(BUILD_DIR)/sysroot
	CC=$(CLANG) LDSHARED="$(CLANG) -pthread -shared" SYSROOT=$(BUILD_DIR)/sysroot python $< build --build-temp=./python_tmp --build-lib $(INTERFACES_DIR)
	cd $(BUILD_DIR)/interfaces && ln -sf rave_user_events.cpython-* rave_user_events.so
	rm -rf ./python_tmp

interfaces: $(INTERFACES_DIR)/rave_user_events_f.o $(INTERFACES_DIR)/rave_user_events.h $(INTERFACES_DIR)/rave_user_events.so

## CLEANING

clean:
	rm -rf $(RAVE_DIR)/obj/* $(LIBDIR)/librave.so
