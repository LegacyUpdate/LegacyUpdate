export SIGN ?= 0

SUBPROJECTS = LegacyUpdate nsisplugin setup

all:
	set -e; \
	for i in $(SUBPROJECTS); do \
		$(MAKE) -j$(shell nproc) -C $$i; \
	done

clean:
	set -e; \
	for i in $(SUBPROJECTS); do \
		$(MAKE) -C $$i clean; \
	done

.PHONY: all clean
