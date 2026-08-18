ifndef BUILD_DIR
  $(error Need to define variable BUILD_DIR)
endif
ifndef RVV
	$(error Need to define variable RVV)
endif
ifndef LLVM_DIR
  $(error Need to provide variable LLVM_DIR)
endif

CC=$(LLVM_DIR)/bin/clang
RAVE_INCLUDE=-I$(BUILD_DIR)/interfaces/rvv

SRCDIR=validation/src
BINDIR=validation/bin/rvv
SCRIPTDIR=validation/scripts

TESTS := test_spec test_multi_page test_lmul_sew_vl test_counters test_profiling common_test_api common_test_nesting test_omp
#.PHONY: $(TESTS) 
all: $(TESTS) 

$(BINDIR): ;
	mkdir -p $@

$(BINDIR)/rvv_spec_$(RVV).x: $(SRCDIR)/rvv/rvv_spec_$(RVV).c | $(BINDIR)
	$(CC) -O3 -mepi -fno-vectorize ${RAVE_INCLUDE} $^ -o $@

test_spec: $(BINDIR)/rvv_spec_$(RVV).x
	${SCRIPTDIR}/rvv/instr_class.sh $(BUILD_DIR)/qemu-rave/RVV-$(RVV)/bin/rave $^

$(BINDIR)/omp_$(RVV).x: $(SRCDIR)/rvv/omp.c | $(BINDIR)
	$(CC) -O3 -mepi ${RAVE_INCLUDE} $^ -o $@ -fopenmp

$(BINDIR)/%_$(RVV).x: $(SRCDIR)/rvv/%.c | $(BINDIR)
	$(CC) -DRVV$(RVV) -O3 -g -mepi -fno-vectorize ${RAVE_INCLUDE} $^ -o $@

$(BINDIR)/%.x: $(SRCDIR)/common/%.c | $(BINDIR)
	$(CC) -O3 -g -fno-vectorize ${RAVE_INCLUDE} $^ -o $@

test_%: $(BINDIR)/%_$(RVV).x
	${SCRIPTDIR}/rvv/$*.sh $(BUILD_DIR)/qemu-rave/RVV-$(RVV)/bin/rave $^

common_test_%: $(BINDIR)/%.x
	${SCRIPTDIR}/common/$*.sh $(BUILD_DIR)/qemu-rave/RVV-$(RVV)/bin/rave $^


clean:
	rm -f ${BINDIR}/*
