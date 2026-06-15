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
RAVE_INCLUDE=-I$(BUILD_DIR)/interfaces 

SRCDIR=validation/src/rvv
BINDIR=validation/bin/rvv
SCRIPTDIR=validation/scripts/rvv

TESTS := test_spec test_multi_page test_lmul_sew_vl test_counters test_profiling test_omp 
#.PHONY: $(TESTS) 
all: $(TESTS) 

$(BINDIR): ;
	mkdir -p $@

$(BINDIR)/rvv_spec_$(RVV).x: $(SRCDIR)/rvv_spec_$(RVV).c | $(BINDIR)
	$(CC) -O3 -mepi -fno-vectorize ${RAVE_INCLUDE} $^ -o $@

test_spec: $(BINDIR)/rvv_spec_$(RVV).x
	${SCRIPTDIR}/instr_class.sh $(BUILD_DIR)/qemu-rave/RVV-$(RVV)/bin/rave $^

$(BINDIR)/omp_$(RVV).x: $(SRCDIR)/omp.c | $(BINDIR)
	$(CC) -O3 -mepi ${RAVE_INCLUDE} $^ -o $@ -fopenmp

$(BINDIR)/%_$(RVV).x: $(SRCDIR)/%.c | $(BINDIR)
	$(CC) -DRVV$(RVV) -O3 -g -mepi -fno-vectorize ${RAVE_INCLUDE} $^ -o $@


test_%: $(BINDIR)/%_$(RVV).x
	${SCRIPTDIR}/$*.sh $(BUILD_DIR)/qemu-rave/RVV-$(RVV)/bin/rave $^


clean:
	rm -f ${BINDIR}/*
