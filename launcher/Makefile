all:
	+$(MAKE) -f Makefile.actual ARCH=32
ifeq ($(CI),1)
	@# HACK: Use the 32-bit binary for nightlies for now, until we have a working 64-bit toolchain
	cp obj/LegacyUpdate32.exe obj/LegacyUpdate64.exe
else
	+$(MAKE) -f Makefile.actual ARCH=64
endif

clean:
	rm -rf obj

.PHONY: all clean
