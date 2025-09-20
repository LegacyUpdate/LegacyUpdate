FILES    = \
	$(wildcard *.c) \
	../shared/Exec.c \
	../shared/HResult.c \
	../shared/LegacyUpdate.c \
	../shared/LoadImage.c \
	../shared/Registry.c \
	../shared/Wow64.c
RCFILES  = resource.rc
DEFFILES = ../LegacyUpdate/LegacyUpdate.def

BIN      = obj/LegacyUpdate$(ARCH).exe

all:: all-archs

all-archs: all-32 all-64

include ../build/shared.mk

LDFLAGS += \
	-Wl,--subsystem,windows \
	-Lobj \
	-lmsvcrt \
	-lpsapi \
	-lkernel32 \
	-luser32 \
	-lole32 \
	-loleaut32 \
	-ladvapi32 \
	-lcomctl32 \
	-lshell32 \
	-lshlwapi \
	-lversion \
	-lgdi32 \
	-lmsimg32

ifeq ($(ARCH),64)
	LDFLAGS += -Wl,-ewWinMain
else
	LDFLAGS += -Wl,-e_wWinMain
endif

test:
	+$(MAKE) DEBUG=$(DEBUG)
	./obj/LegacyUpdate.exe

.PHONY: all all-archs all-32 all-64 test
