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

ARCH   ?= 32

ifeq ($(ARCH),64)
TAG     = x86_64
else
TAG     = i686
endif

PREFIX  = $(TAG)-w64-mingw32-

BIN     = obj/LegacyUpdate$(ARCH).exe
OBJ     = $(foreach file,$(FILES),obj/$(notdir $(basename $(file)).$(TAG).o))
RES     = $(foreach file,$(RCFILES),obj/$(notdir $(basename $(file)).$(TAG).res))
DEFDLL  = $(foreach file,$(DEFFILES),obj/lib$(notdir $(basename $(file)).$(TAG).a))

CC      = $(PREFIX)g++
RC      = $(PREFIX)windres
DLLTOOL = $(PREFIX)dlltool

override DEBUG := $(or $(DEBUG),1)

CFLAGS = \
	-mwindows \
	-municode \
	-DUNICODE \
	-D_UNICODE \
	$(if $(filter 1,$(DEBUG)),-D_DEBUG -g,-DNDEBUG -Os) \
	-s \
	-fPIE \
	-ffunction-sections \
	-fdata-sections \
	-fno-unwind-tables \
	-fno-asynchronous-unwind-tables \
	-fno-exceptions \
	-flto \
	-Wall \
	-Werror \
	-Wextra \
	-Wpedantic \
	-Wno-unused-parameter \
	-Wno-unknown-pragmas \
	-Wno-cast-function-type \
	-I../include \
	-I../shared \
	-include stdafx.h

CXXFLAGS = \
	$(CFLAGS) \
	-std=c++11 \
	-fno-rtti

LDFLAGS = \
	-nodefaultlibs \
	-nostartfiles \
	-nostdlib \
	-Wl,--gc-sections \
	-Wl,--no-seh \
	-Wl,--nxcompat \
	-Wl,--enable-auto-image-base \
	-Wl,--enable-stdcall-fixup \
	-Wl,--strip-all \
	-Lobj \
	-lmsvcrt \
	-lgcc \
	-lpsapi \
	-lkernel32 \
	-luser32 \
	-lole32 \
	-loleaut32 \
	-ladvapi32 \
	-lcomctl32 \
	-lshell32 \
	-lversion \
	-lgdi32 \
	-lmsimg32

RCFLAGS = \
	-O coff \
	-I../shared

ifeq ($(ARCH),64)
	LDFLAGS += -Wl,-ewWinMain
	RCFLAGS += -F pe-x86-64
else
	CFLAGS  += -march=i486
	LDFLAGS += -Wl,-e_wWinMain
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
	@mkdir -p obj
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

obj/%.$(TAG).o: %.c
	@mkdir -p obj
	$(CC) -x c $< $(CFLAGS) -c -o $@

obj/%.$(TAG).o: ../shared/%.c
	@mkdir -p obj
	$(CC) -x c $< $(CFLAGS) -c -o $@

obj/%.$(TAG).res: %.rc
	@mkdir -p obj
	$(RC) $< $(RCFLAGS) -o $@

obj/lib%.$(TAG).a: ../LegacyUpdate/%.def
	@mkdir -p obj
	$(DLLTOOL) -d $< -l $@ -D $(notdir $(basename $<).dll)

clean:
	rm -rf obj

test:
	+$(MAKE) DEBUG=$(DEBUG)
	./obj/LegacyUpdate.exe

.PHONY: all all-archs all-32 all-64 internal-all clean test
