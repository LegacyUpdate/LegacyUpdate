export CI ?= 0
export SIGN ?= 0
export DEBUG ?= 1

all:
ifeq ($(DEBUG),0)
	+$(MAKE) clean
endif

	+$(MAKE) -C LegacyUpdate
	+$(MAKE) -C launcher
	+$(MAKE) -C setup

nt4:
ifeq ($(DEBUG),0)
	+$(MAKE) clean-nt4
endif

	+$(MAKE) -C setup nt4

clean:
	+$(MAKE) -C LegacyUpdate clean
	+$(MAKE) -C launcher clean
	+$(MAKE) -C setup clean

clean-nt4:
	+$(MAKE) -C setup clean

.PHONY: all nt4 clean clean-nt4
