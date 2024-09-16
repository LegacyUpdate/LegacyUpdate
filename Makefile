export SIGN ?= 0
export DEBUG ?= 1

all:
ifeq ($(DEBUG),0)
	+$(MAKE) clean
endif

	+$(MAKE) -C LegacyUpdate
	+$(MAKE) -C launcher
	+$(MAKE) -C setup

clean:
	+$(MAKE) -C LegacyUpdate clean
	+$(MAKE) -C launcher clean
	+$(MAKE) -C setup clean

.PHONY: all clean
