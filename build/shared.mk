_COMMA = ,

CLANG  ?= 0
ARCH   ?= 32

ifeq ($(ARCH),64)
TAG     = x86_64
else
TAG     = i686
endif

PREFIX  = $(TAG)-w64-mingw32-

EXT     = $(if $(findstring .exe,$(BIN)),exe,dll)

ifeq ($(DEF),)
DEF     = $(patsubst %.$(EXT),%.def,$(BIN))
EMITDEF = 1
endif

STATIC  = $(patsubst %.$(EXT),%.a,$(BIN))

OBJDIR  = obj/$(TAG)

OBJ     = $(foreach file,$(FILES),$(OBJDIR)/$(notdir $(basename $(file)).o))
DEFDLL  = $(foreach file,$(DEFFILES),$(OBJDIR)/lib$(notdir $(basename $(file)).a))
RES     = $(foreach file,$(RCFILES),$(OBJDIR)/$(notdir $(basename $(file)).res))
MSG     = $(foreach file,$(MCFILES),$(OBJDIR)/$(notdir $(basename $(file)).bin))

IDL     = $(foreach file,$(IDLFILES),$(OBJDIR)/$(notdir $(basename $(file))_i.c))
IDL_H   = $(foreach file,$(IDLFILES),$(OBJDIR)/$(notdir $(basename $(file))_i.h))
IDL_P   = $(foreach file,$(IDLFILES),$(OBJDIR)/$(notdir $(basename $(file))_p.c))
DLLDATA = $(foreach file,$(IDLFILES),$(OBJDIR)/$(notdir $(basename $(file))_dlldata.c))
TLB     = $(foreach file,$(IDLFILES),$(OBJDIR)/$(notdir $(basename $(file)).tlb))

ifeq ($(CLANG),1)
	CC    = $(PREFIX)clang++
else
	CC    = $(PREFIX)gcc
endif

RC      = $(PREFIX)windres
MC      = $(PREFIX)windmc
WIDL    = $(PREFIX)widl
DLLTOOL = $(PREFIX)dlltool

C_LANG   ?= c
CXX_LANG ?= c++

override DEBUG := $(or $(DEBUG),1)

CFLAGS   += \
	-municode \
	-DUNICODE \
	-D_UNICODE \
	$(if $(filter 1,$(DEBUG)),-D_DEBUG,-DNDEBUG -Os -s) \
	-g \
	-fPIE \
	-ffunction-sections \
	-fdata-sections \
	-fno-unwind-tables \
	-fno-asynchronous-unwind-tables \
	-fno-exceptions \
	$(if $(EMITDEF),-flto,) \
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
	-I$(OBJDIR) \
	-include stdafx.h

ifeq ($(CLANG),1)
	CFLAGS += \
		-Wno-dollar-in-identifier-extension \
		-Wno-missing-braces \
		-Wno-duplicate-decl-specifier
endif

CXXFLAGS += \
	$(CFLAGS) \
	-std=c++11 \
	-fno-rtti

LDFLAGS  += \
	-nodefaultlibs \
	-nostartfiles \
	-nostdlib \
	-Wl,--gc-sections \
	-Wl,--no-seh \
	-Wl,--nxcompat \
	-Wl,--enable-auto-image-base \
	-Wl,--enable-stdcall-fixup \
	-Wl,--out-implib,$(STATIC) \
	$(if $(EMITDEF),-Wl$(_COMMA)--output-def$(_COMMA)$(DEF),$(DEF)) \
	$(if $(filter 1,$(DEBUG)),,-Wl$(COMMA)--strip-all) \
	-lmsvcrt

ifeq ($(CLANG),1)
	LDFLAGS += \
		-static
else
	LDFLAGS += \
		-lgcc
endif

RCFLAGS  += \
	-O coff \
	-I$(OBJDIR) \
	-I../shared

MCFLAGS  +=

IDLFLAGS += \
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

all-32:
	+$(MAKE) internal-all ARCH=32

all-64:
	+$(MAKE) internal-all ARCH=64

internal-all:: $(BIN)
ifeq ($(SIGN),1)
	../build/sign.sh $(BIN)
endif

$(BIN): $(OBJ) $(RES)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

$(DEF): ;

$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/lib%.a: ../LegacyUpdate/%.def
	$(DLLTOOL) -d $< -l $@ -D $(notdir $(basename $<).dll)

$(OBJDIR)/%.o: %.c
	$(CC) -x $(C_LANG) $< $(CFLAGS) -c -o $@

$(OBJDIR)/%.o: %.cpp
	$(CC) -x $(CXX_LANG) $< $(CXXFLAGS) -c -o $@

$(OBJDIR)/%.o: ../shared/%.c
	$(CC) -x $(C_LANG) $< $(CFLAGS) -c -o $@

$(OBJDIR)/%.o: ../shared/%.cpp
	$(CC) -x $(CXX_LANG) $< $(CXXFLAGS) -c -o $@

$(OBJDIR)/%.o: $(OBJDIR)/%.c
	$(CC) -x $(C_LANG) $< $(CFLAGS) -c -o $@

$(OBJDIR)/%.o: $(OBJDIR)/%.c
	$(CC) -x $(C_LANG) $< $(CFLAGS) -c -o $@

$(OBJDIR)/%.res: %.rc
	$(RC) $< $(RCFLAGS) -o $@

$(OBJDIR)/%.bin: %.mc
	$(MC) $< $(MCFLAGS) -h $(dir $@) -r $(dir $@)

$(OBJDIR)/%.tlb: %.idl | $(OBJDIR)
	$(WIDL) $(IDLFLAGS) -t -o $@ $<

$(OBJDIR)/%_i.h: %.idl | $(OBJDIR)
	$(WIDL) $(IDLFLAGS) -h -o $@ $<

$(OBJDIR)/%_i.c: %.idl | $(OBJDIR)
	$(WIDL) $(IDLFLAGS) -u -o $@ $<

$(OBJDIR)/%_p.c: %.idl | $(OBJDIR)
	$(WIDL) $(IDLFLAGS) -p -o $@ $<

$(OBJDIR)/%_dlldata.c: %.idl | $(OBJDIR)
	$(WIDL) $(IDLFLAGS) --dlldata-only -o $@ $(notdir $(basename $<))

$(OBJ): stdafx.h $(IDL_H)

$(OBJ) $(RES) $(MSG): | $(OBJDIR)

$(RES): $(MSG) $(TLB)

clean:
	rm -rf obj

.PHONY: all-32 all-64 internal-all clean
