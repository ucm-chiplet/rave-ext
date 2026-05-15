ifndef BUILD_DIR
	$(error Need to define vairable BUILD_DIR)
endif
ifndef RVV
	$(error Need to define vairable RVV)
endif

QEMU_REPO=https://gitlab.com/qemu-project/qemu.git
ifeq ($(RVV),0_7_1)
  QEMU_BRANCH=stable-6.1
else
  QEMU_BRANCH = stable-10.0
  #QEMU_BRANCH = stable-9.0
endif

QEMU_BUILD_DIR=$(BUILD_DIR)/qemu-rave/RVV-$(RVV)
QEMU_BUILD_TARGET=$(QEMU_BUILD_DIR)/qemu/bin/qemu-riscv64
QEMU_SOURCE_DIR=downloads/qemu-rvv-$(RVV)

QEMU_BUILD_OPTIONS=--target-list=riscv64-linux-user --disable-docs --prefix=$(QEMU_BUILD_DIR)/qemu --enable-plugins
ifeq ($(RVV),0_7_1)
QEMU_BUILD_OPTIONS+=--disable-werror 
#else
#QEMU_BUILD_OPTIONS+=--python=python3.12
endif
QEMU_MAXVLEN=256*64*1 #16384
QEMU_SIMD_BITS=8+3 #2^bits is max VL at sew64.  +3 is for LMUL 8. 2^(8+3)=2048 (256*8)

.PHONY: clean_qemu 

all: $(QEMU_BUILD_TARGET)

$(QEMU_BUILD_DIR): ;
	mkdir -p $@

$(QEMU_SOURCE_DIR): ;
	@mkdir -p $@
	git clone $(QEMU_REPO) -b $(QEMU_BRANCH) $@

$(QEMU_SOURCE_DIR)/.configured: | $(QEMU_SOURCE_DIR) $(QEMU_BUILD_DIR)
	cd $(QEMU_SOURCE_DIR) && git restore . && git clean -f #&& make clean ; make distclean
	cd $(QEMU_SOURCE_DIR) && ./configure $(QEMU_BUILD_OPTIONS) 
	touch $@

$(QEMU_SOURCE_DIR)/.patched: $(QEMU_SOURCE_DIR)/.configured
	cd $(QEMU_SOURCE_DIR) && \
	sed -i "s/^\#define\ RV_VLEN_MAX\ .*/\#define\ RV_VLEN_MAX\ \($(QEMU_MAXVLEN)\)/g" ./target/riscv/cpu.h  && \
	sed -i "s/^\#define\ SIMD_MAXSZ_BITS\ .*/\#define\ SIMD_MAXSZ_BITS\ \($(QEMU_SIMD_BITS)\)/g" ./include/tcg/tcg-gvec-desc.h && \
	patch -p 1 < ../../utils/translate_$(RVV).patch && \
	patch -p 0 < ../../utils/multipage_$(RVV).patch && \
	if [ "$(RVV)" = "0_7_1" ]; then \
		sed -i '/};/i cpu_memory_rw_debug;' plugins/qemu-plugins.symbols && \
		sed -i '/};/i qemu_get_cpu;' plugins/qemu-plugins.symbols && \
 		patch -p 0 < ../../utils/expose_$(RVV).patch ; \
  fi
	touch $@


NUM_JOBS=12
$(QEMU_BUILD_TARGET): $(QEMU_SOURCE_DIR)/.patched
	cd $(QEMU_SOURCE_DIR) && make -j $(NUM_JOBS) && make -j $(NUM_JOBS) install

clean: 
	rm -rf $(QEMU_BUILD_DIR) $(QEMU_SOURCE_DIR)
