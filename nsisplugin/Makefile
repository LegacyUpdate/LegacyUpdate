FILES    = \
	$(wildcard *.c) \
	../shared/HResult.c \
	../shared/LegacyUpdate.c \
	../shared/LoadImage.c \
	../shared/Registry.c \
	../shared/Wow64.c
RCFILES  = resource.rc

BIN      = obj/LegacyUpdateNSIS.dll

all: internal-all

include ../build/shared.mk

CFLAGS  += \
	-mdll

LDFLAGS += \
	-Wl,-e_DllMain \
	-lpsapi \
	-lkernel32 \
	-luser32 \
	-lole32 \
	-loleaut32 \
	-ladvapi32 \
	-lgdi32 \
	-lmsimg32 \
	-lcrypt32

internal-all::
	cp $(BIN) ../setup/x86-unicode/

test:
	+$(MAKE) DEBUG=$(DEBUG)
	cd ../setup && makensis test.nsi
	cd ../setup && explorer.exe test.exe

.PHONY: all test
