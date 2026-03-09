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

SRCDIR=validation/src
BINDIR=validation/bin
SCRIPTDIR=validation/scripts

TESTS = test_spec test_multipage test_lmul_sew_vl
.PHONY: $(TESTS) 
all: $(TESTS) 

$(BINDIR): ;
	mkdir -p $@

$(BINDIR)/rvv_spec_$(RVV).x: $(SRCDIR)/rvv_spec_$(RVV).c | $(BINDIR)
	$(CC) -O3 -mepi -fno-vectorize ${RAVE_INCLUDE} $^ -o $@

$(BINDIR)/multipage_$(RVV).x: $(SRCDIR)/multi_page.c | $(BINDIR)
	$(CC) -DRVV$(RVV) -O3 -mepi -fno-vectorize ${RAVE_INCLUDE} $^ -o $@

$(BINDIR)/lmul_sew_vl_$(RVV).x: $(SRCDIR)/lmul_sew_vl.c | $(BINDIR)
	$(CC) -DRVV$(RVV) -O3 -mepi -fno-vectorize ${RAVE_INCLUDE} $^ -o $@

test_spec: $(BINDIR)/rvv_spec_$(RVV).x
	${SCRIPTDIR}/instr_class.sh $(BUILD_DIR)/qemu-rave/RVV-$(RVV)/bin/rave $^

test_multipage: $(BINDIR)/multipage_$(RVV).x
	${SCRIPTDIR}/multipage.sh $(BUILD_DIR)/qemu-rave/RVV-$(RVV)/bin/rave $^

test_lmul_sew_vl: $(BINDIR)/lmul_sew_vl_$(RVV).x
	${SCRIPTDIR}/lmul_sew_vl.sh $(BUILD_DIR)/qemu-rave/RVV-$(RVV)/bin/rave $^

clean:
	rm -f ${BINDIR}/*
