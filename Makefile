export SIGN ?= 0

SUBPROJECTS = LegacyUpdate nsisplugin setup

all:
	set -e; \
	for i in $(SUBPROJECTS); do \
		$(MAKE) -C $$i; \
	done

.PHONY: all
