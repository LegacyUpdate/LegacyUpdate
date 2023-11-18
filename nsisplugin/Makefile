FILES   = \
	$(wildcard *.c) \
	$(wildcard *.cpp) \
	../include/nsis/pluginapi.c \
	../Shared/HResult.c \
	../Shared/WUErrors.c
RCFILES = resource.rc

BIN     = obj/LegacyUpdateNSIS.dll
DEF     = $(patsubst %.dll,%.def,$(BIN))
STATIC  = $(patsubst %.dll,%.a,$(BIN))
OBJ     = $(foreach file,$(FILES),obj/$(notdir $(basename $(file)).o))
RES     = $(foreach file,$(RCFILES),obj/$(notdir $(basename $(file)).res))

PREFIX  = i686-w64-mingw32-

CC = $(PREFIX)g++
RC = $(PREFIX)windres

CFLAGS = \
	-std=c++11 \
	-march=i486 \
	-mdll \
	-municode \
	-DUNICODE \
	-D_UNICODE \
	$(if $(DEBUG),-D_DEBUG -g,-DNDEBUG -Os) \
	-D__USE_MINGW_ANSI_STDIO=0 \
	-D_USRDLL \
	-s \
	-fPIE \
	-ffunction-sections \
	-fdata-sections \
	-fno-unwind-tables \
	-fno-asynchronous-unwind-tables \
	-Wno-write-strings \
	-I../include \
	-I../shared \
	-include stdafx.h

LDFLAGS = \
	-static \
	-Wl,--gc-sections \
	-Wl,--no-seh \
	-Wl,--nxcompat \
	-Wl,--enable-auto-image-base \
	-Wl,--enable-stdcall-fixup \
	-Wl,--output-def,$(DEF) \
	-Wl,--out-implib,$(STATIC) \
	-Wl,-e_DllMain \
	-lpsapi \
	-lkernel32 \
	-luser32 \
	-lole32 \
	-loleaut32 \
	-lwbemuuid

RCFLAGS = \
	-F pe-i386 \
	-O coff

all: before-all $(BIN) after-all

before-all:
	mkdir -p obj

after-all:
	cp $(BIN) ../setup/x86-unicode/

$(BIN): $(OBJ) $(RES)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

obj/%.o: %.c
	$(CC) $< $(CFLAGS) -c -o $@

obj/%.o: %.cpp
	$(CC) $< $(CFLAGS) -c -o $@

obj/%.o: ../shared/%.c
	$(CC) $< $(CFLAGS) -c -o $@

obj/%.o: ../shared/%.cpp
	$(CC) $< $(CFLAGS) -c -o $@

obj/%.o: ../include/nsis/%.c
	$(CC) $< $(CFLAGS) -c -o $@

obj/%.res: %.rc
	$(RC) $< $(RCFLAGS) -o $@

clean:
	rm -rf obj

test:
	$(MAKE) DEBUG=$(or $(DEBUG),1)
	cd ../setup && makensis test.nsi
	cd ../setup && explorer.exe test.exe

.PHONY: all before-all after-all clean test
