FILES    = \
	$(wildcard *.cpp) \
	dlldatax.c \
	../shared/Exec.c \
	../shared/HResult.c \
	../shared/LegacyUpdate.c \
	../shared/Registry.c \
	../shared/WMI.c \
	../shared/Wow64.c \
	$(OBJDIR)/LegacyUpdate_i.c

RCFILES  = LegacyUpdate.rc
MCFILES  = wuerror.mc
IDLFILES = LegacyUpdate.idl

BIN      = obj/LegacyUpdate$(ARCH).dll
DEF      = LegacyUpdate.def

C_LANG   = c++
CXX_LANG = c++

all:: all-archs

all-archs: all-32 all-64

include ../build/shared.mk

LDFLAGS += \
	-shared \
	-lkernel32 \
	-luser32 \
	-lgdi32 \
	-lole32 \
	-loleaut32 \
	-ladvapi32 \
	-lshell32 \
	-lcomctl32 \
	-lpsapi \
	-lwbemuuid \
	-luuid \
	-lshlwapi \
	-lrpcrt4 \
	-lrpcns4

ifeq ($(ARCH),64)
	LDFLAGS += -Wl,-eDllMain
else
	LDFLAGS += -Wl,-e_DllMain
endif

$(OBJ): $(OBJDIR)/LegacyUpdate_i.h

$(OBJDIR)/dlldatax.o: dlldatax.c $(DLLDATA) $(IDL_P)
	$(CC) -x c $< $(filter-out -Wpedantic, $(CFLAGS)) -c -o $@

.PHONY: all all-archs all-32 all-64
