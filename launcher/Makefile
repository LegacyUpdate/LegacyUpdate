all:
	$(MAKE) -f Makefile.actual -j$(shell nproc) ARCH=32
	$(MAKE) -f Makefile.actual -j$(shell nproc) ARCH=64

clean:
	rm -rf obj

.PHONY: all clean
