ifndef BUILD_DIR
	$(error Need to define variable BUILD_DIR)
endif
ifndef LLVM_DIR
  $(error Need to provide variable LLVM_DIR)
endif

SRCDIR=rave_sources/parallel
LIBDIR=$(BUILD_DIR)/parallel
CLANG=$(LLVM_DIR)/bin/clang

all: $(LIBDIR)/ompt.so $(LIBDIR)/libomp.so

$(LIBDIR): ;
	mkdir -p $@

$(LIBDIR)/ompt.so: $(SRCDIR)/ompt/ompt.c | $(LIBDIR)
	$(CLANG) -O3 $< -shared -fPIC -o $@

$(LIBDIR)/libomp.so: $(SRCDIR)/lib/libomp.so | $(LIBDIR)
	cp $^ $@

clean: 
	rm -rf $(LIBDIR)
