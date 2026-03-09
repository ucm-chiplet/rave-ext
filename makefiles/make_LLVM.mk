ifndef BUILD_DIR
	$(error Need to define vairable BUILD_DIR)
endif
ifndef RVV
  $(error Need to define variable RVV)
endif

LLVM_BUILD_DIR=$(BUILD_DIR)/llvm-cross

EPI_FTP=https://ssh.hca.bsc.es/epi/ftp
ifeq ($(RVV),0_7_1)
  LLVM_TAR=llvm-EPI-0.7-development-toolchain-cross-latest.tar.bz2
  LLVM_SUBBUILD_DIR=$(LLVM_BUILD_DIR)/llvm-EPI-0.7-development-toolchain-cross
else
  LLVM_TAR=llvm-EPI-development-toolchain-cross-latest.tar.bz2
  LLVM_SUBBUILD_DIR=$(LLVM_BUILD_DIR)/llvm-EPI-development-toolchain-cross
endif

LLVM_BUILD_TARGET=$(LLVM_SUBBUILD_DIR)/bin/clang

LLVM_SRC_DIR=downloads

all: $(LLVM_BUILD_TARGET) 

$(LLVM_SRC_DIR): ;
	mkdir -p $@

$(LLVM_BUILD_DIR): ;
	mkdir -p $@

$(LLVM_SRC_DIR)/$(LLVM_TAR): | $(LLVM_SRC_DIR)
	cd $(LLVM_SRC_DIR) && wget $(EPI_FTP)/$(LLVM_TAR)

$(LLVM_BUILD_TARGET): $(LLVM_SRC_DIR)/$(LLVM_TAR) | $(LLVM_BUILD_DIR)
	tar -xf $^ -C $(LLVM_BUILD_DIR)
	touch $@

clean:
	rm -rf $(LLVM_SRC_DIR) $(LLVM_SUBBUILD_DIR)
