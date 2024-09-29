FILES   = \
	$(wildcard *.c) \
	../include/nsis/pluginapi.c \
	../Shared/HResult.c \
	../Shared/LoadImage.c \
	../Shared/GetPidForName.c \
	../Shared/WMI.c \
	../Shared/WUErrors.c
RCFILES = resource.rc

TAG     = i686
PREFIX  = i686-w64-mingw32-

BIN     = obj/LegacyUpdateNSIS.dll
DEF     = $(patsubst %.dll,%.def,$(BIN))
STATIC  = $(patsubst %.dll,%.a,$(BIN))
OBJ     = $(foreach file,$(FILES),obj/$(notdir $(basename $(file)).$(TAG).o))
RES     = $(foreach file,$(RCFILES),obj/$(notdir $(basename $(file)).$(TAG).res))

CC = $(PREFIX)g++
RC = $(PREFIX)windres

override DEBUG := $(or $(DEBUG),1)

CFLAGS = \
	-march=i486 \
	-mdll \
	-municode \
	-DUNICODE \
	-D_UNICODE \
	$(if $(filter 1,$(DEBUG)),-D_DEBUG -g,-DNDEBUG -Os) \
	-D_USRDLL \
	-s \
	-fPIE \
	-ffunction-sections \
	-fdata-sections \
	-fno-unwind-tables \
	-fno-asynchronous-unwind-tables \
	-fno-exceptions \
	-flto \
	-Wno-write-strings \
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
	-Wl,--output-def,$(DEF) \
	-Wl,--out-implib,$(STATIC) \
	-Wl,--strip-all \
	-Wl,-e_DllMain \
	-lmsvcrt \
	-lgcc \
	-lpsapi \
	-lkernel32 \
	-luser32 \
	-lole32 \
	-loleaut32 \
	-ladvapi32 \
	-lgdi32 \
	-lmsimg32

RCFLAGS = \
	-F pe-i386 \
	-O coff

all: before-all $(BIN) after-all
ifeq ($(SIGN),1)
	../build/sign.sh $(BIN)
endif
	cp $(BIN) ../setup/x86-unicode/

before-all:
	mkdir -p obj

$(BIN): $(OBJ) $(RES)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

obj/%.$(TAG).o: %.c
	$(CC) -x c $< $(CFLAGS) -c -o $@

obj/%.$(TAG).o: %.cpp
	$(CC) -x c++ $< $(CXXFLAGS) -c -o $@

obj/%.$(TAG).o: ../shared/%.c
	$(CC) -x c $< $(CFLAGS) -c -o $@

obj/%.$(TAG).o: ../shared/%.cpp
	$(CC) -x c++ $< $(CXXFLAGS) -c -o $@

obj/%.$(TAG).o: ../include/nsis/%.c
	$(CC) -x c $< $(CFLAGS) -c -o $@

obj/%.$(TAG).res: %.rc
	$(RC) $< $(RCFLAGS) -o $@

clean:
	rm -rf obj

test:
	+$(MAKE) DEBUG=$(DEBUG)
	cd ../setup && makensis test.nsi
	cd ../setup && explorer.exe test.exe

.PHONY: all before-all after-all clean test
