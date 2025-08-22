FILES = \
	$(wildcard *.cpp) \
	dlldatax.c \
	../shared/Exec.c \
	../shared/HResult.c \
	../shared/LegacyUpdate.c \
	../shared/Registry.c \
	../shared/WMI.c \
	../shared/Wow64.c \
	obj/$(TAG)/LegacyUpdate_i.c

RCFILES  = LegacyUpdate.rc
MCFILES  = wuerror.mc
IDLFILES = LegacyUpdate.idl

ARCH   ?= 32

ifeq ($(ARCH),64)
TAG     = x86_64
else
TAG     = i686
endif

PREFIX  = $(TAG)-w64-mingw32-

BIN     = obj/LegacyUpdate$(ARCH).dll
DEF     = LegacyUpdate.def
STATIC  = $(patsubst %.dll,%.a,$(BIN))
OBJ     = $(foreach file,$(FILES),obj/$(notdir $(basename $(file)).$(TAG).o))
RES     = $(foreach file,$(RCFILES),obj/$(notdir $(basename $(file)).$(TAG).res))
MSG     = $(foreach file,$(MCFILES),obj/$(notdir $(basename $(file)).bin))
IDL     = $(foreach file,$(IDLFILES),obj/$(TAG)/$(notdir $(basename $(file))_i.c))

CC      = $(PREFIX)gcc
RC      = $(PREFIX)windres
MC      = $(PREFIX)windmc
WIDL    = $(PREFIX)widl

override DEBUG := $(or $(DEBUG),1)

CFLAGS = \
	-mdll \
	-municode \
	-DUNICODE \
	-D_UNICODE \
	$(if $(filter 1,$(DEBUG)),-D_DEBUG -g,-DNDEBUG -Os) \
	-fPIE \
	-fno-unwind-tables \
	-fno-asynchronous-unwind-tables \
	-fno-exceptions \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Wno-unused-parameter \
	-Wno-unused-variable \
	-Wno-unknown-pragmas \
	-Wno-cast-function-type \
	-Wno-missing-field-initializers \
	-I../include \
	-I../shared \
	-Iobj \
	-Iobj/$(TAG) \
	-include stdafx.h

CXXFLAGS = \
	$(CFLAGS) \
	-std=c++11 \
	-fno-rtti

LDFLAGS = \
	-Wl,--no-seh \
	-Wl,--nxcompat \
	-Wl,--enable-auto-image-base \
	-Wl,--enable-stdcall-fixup \
	-Wl,--out-implib,$(STATIC) \
	$(DEF) \
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

RCFLAGS = \
	-O coff \
	-Iobj \
	-Iobj/$(TAG) \
	-I../shared \

MCFLAGS =

IDLFLAGS = \
	-m$(ARCH) \
	--win$(ARCH) \
	-Oicf \
	-Ishared \
	-I/usr/include/wine/wine/windows \
	-L/usr/lib/x86_64-linux-gnu/wine/x86_64-windows

ifeq ($(ARCH),64)
	RCFLAGS += -F pe-x86-64
else
	CFLAGS  += -march=i486
	RCFLAGS += -F pe-i386
endif

all: all-archs

all-archs: all-32 all-64

all-32:
	+$(MAKE) internal-all ARCH=32

all-64:
	+$(MAKE) internal-all ARCH=64

internal-all: $(BIN)
ifeq ($(SIGN),1)
	../build/sign.sh $(BIN)
endif

$(BIN): $(OBJ) $(RES)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

$(DEF): ;

obj/$(TAG):
	mkdir -p $@

obj/%.$(TAG).o: %.c
	$(CC) -x c $< $(CFLAGS) -c -o $@

obj/dlldatax.$(TAG).o: dlldatax.c
	$(CC) -x c $< $(filter-out -Wpedantic, $(CFLAGS)) -c -o $@

obj/%.$(TAG).o: %.cpp
	$(CC) -x c++ $< $(CXXFLAGS) -c -o $@

obj/%.$(TAG).o: ../shared/%.c
	$(CC) -x c++ $< $(CFLAGS) -c -o $@

obj/%.$(TAG).o: ../shared/%.cpp
	$(CC) -x c++ $< $(CXXFLAGS) -c -o $@

obj/%.$(TAG).o: obj/%.c
	$(CC) -x c++ $< $(CFLAGS) -c -o $@

obj/%.$(TAG).o: obj/$(TAG)/%.c
	$(CC) -x c++ $< $(CFLAGS) -c -o $@

obj/%.$(TAG).res: %.rc
	$(RC) $< $(RCFLAGS) -o $@

obj/%.bin: %.mc
	$(MC) $< $(MCFLAGS) -h $(dir $@) -r $(dir $@)

obj/$(TAG)/%.tlb: %.idl | obj/$(TAG)
	$(WIDL) $(IDLFLAGS) -t -o $@ $<

obj/$(TAG)/%_i.h: %.idl | obj/$(TAG)
	$(WIDL) $(IDLFLAGS) -h -o $@ $<

obj/$(TAG)/%_i.c: %.idl | obj/$(TAG)
	$(WIDL) $(IDLFLAGS) -u -o $@ $<

obj/$(TAG)/%_p.c: %.idl | obj/$(TAG)
	$(WIDL) $(IDLFLAGS) -p -o $@ $<

obj/$(TAG)/%_dlldata.c: %.idl | obj/$(TAG)
	$(WIDL) $(IDLFLAGS) --dlldata-only -o $@ $(notdir $(basename $<))

$(OBJ): obj/$(TAG)/LegacyUpdate_i.h stdafx.h

$(OBJ) $(RES) $(MSG): | obj/$(TAG)

$(RES): $(MSG)

obj/dlldatax.$(TAG).o: \
	obj/$(TAG)/LegacyUpdate_dlldata.c \
	obj/$(TAG)/LegacyUpdate_p.c

obj/LegacyUpdate.$(TAG).res: obj/$(TAG)/LegacyUpdate.tlb

clean:
	rm -rf obj

.PHONY: all all-archs all-32 all-64 internal-all clean
