# Requires the libxenon toolchain (https://github.com/Free60Project/libxenon)
# and DEVKITXENON set, e.g. export DEVKITXENON=/usr/local/xenon

ifeq ($(strip $(DEVKITXENON)),)
$(error Set DEVKITXENON, e.g. export DEVKITXENON=/usr/local/xenon)
endif

TARGET   := hello360
CXX      := $(DEVKITXENON)/bin/xenon-g++
OBJCOPY  := $(DEVKITXENON)/bin/xenon-objcopy
STRIP    := $(DEVKITXENON)/bin/xenon-strip

MACHDEP  := -DXENON -m32 -mno-altivec -fno-pic -mpowerpc64 -mhard-float
CXXFLAGS := -O2 -Wall $(MACHDEP) -I$(DEVKITXENON)/usr/include
LDFLAGS  := $(MACHDEP) -n -nostartfiles -Wl,--gc-sections \
            -L$(DEVKITXENON)/usr/lib -L$(DEVKITXENON)/xenon/lib/32 \
            -T$(DEVKITXENON)/app.lds
LIBS     := -lxenon -lm

all: $(TARGET).elf32

$(TARGET).elf: main.o
	$(CXX) $^ $(LDFLAGS) $(LIBS) -o $@

# elf32 is what XeLL loads
$(TARGET).elf32: $(TARGET).elf
	$(OBJCOPY) -O elf32-powerpc --adjust-vma 0x80000000 $< $@
	$(STRIP) $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o *.elf *.elf32
