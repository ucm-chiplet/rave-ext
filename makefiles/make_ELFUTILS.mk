ifndef BUILD_DIR
	$(error Need to define vairable BUILD_DIR)
endif

ELFUTILS_REPO=git://sourceware.org/git/elfutils.git
SRCDIR=downloads/elfutils
ELFUTILS_BUILD_DIR=$(BUILD_DIR)/elfutils
BUILD_TARGET=$(ELFUTILS_BUILD_DIR)/lib/libdw.so

all: $(BUILD_TARGET)

$(SRCDIR): ;
	git clone $(ELFUTILS_REPO) $@

$(SRCDIR)/Makefile: | $(SRCDIR)
	cd $(SRCDIR) && autoreconf -i -f && ./configure --prefix=$(ELFUTILS_BUILD_DIR) --enable-maintainer-mode 

$(BUILD_TARGET): $(SRCDIR)/Makefile
	cd $(SRCDIR) && make install -j 4 dwarf_cu_dwp_section_info_no_Werror=1 link_map_no_Werror=1

clean:
	rm -rf $(SRCDIR) $(ELFUTILS_BUILD_DIR)
