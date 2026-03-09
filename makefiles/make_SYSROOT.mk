ifndef BUILD_DIR
	$(error Need to define vairable BUILD_DIR)
endif

EPI_FTP=https://ssh.hca.bsc.es/epi/ftp/RAVE
SRCDIR=downloads/sysroot.tar.gz
BUILD_TARGET=$(BUILD_DIR)/sysroot/lib/ld-linux-riscv64-lp64d.so.1

$(info $(BUILD_TARGET))
all: $(BUILD_TARGET)

$(SRCDIR): ;
	cd downloads && wget $(EPI_FTP)/sysroot.tar.gz

$(BUILD_TARGET) : $(SRCDIR)
	tar -xzf $^ -C $(BUILD_DIR)
	touch $@

clean:
	rm -rf $(SRCDIR) $(BUILD_DIR)/sysroot
