# hello360 - needs the libxenon toolchain.
#   export DEVKITXENON=/usr/local/xenon
#   export PATH=$PATH:$DEVKITXENON/bin

ifeq ($(strip $(DEVKITXENON)),)
$(error DEVKITXENON is not set. Run: export DEVKITXENON=/usr/local/xenon)
endif

TARGET   := hello360
SRCS     := main.cpp
OBJS     := $(SRCS:.cpp=.o)
DEPS     := $(OBJS:.o=.d)

CXX      := $(DEVKITXENON)/bin/xenon-g++
OBJCOPY  := $(DEVKITXENON)/bin/xenon-objcopy
STRIP    := $(DEVKITXENON)/bin/xenon-strip

# Fail early with a clear message if the toolchain isn't built yet
ifeq ($(wildcard $(CXX)),)
$(error Cannot find $(CXX). Build the libxenon toolchain first)
endif

MACHDEP  := -DXENON -m32 -mno-altivec -fno-pic -mpowerpc64 -mhard-float
CXXFLAGS := -O2 -Wall -Wextra -fno-exceptions -fno-rtti -MMD -MP \
            $(MACHDEP) -I$(DEVKITXENON)/usr/include
LDFLAGS  := $(MACHDEP) -n -nostartfiles -Wl,--gc-sections \
            -L$(DEVKITXENON)/usr/lib -L$(DEVKITXENON)/xenon/lib/32 \
            -T$(DEVKITXENON)/app.lds
LIBS     := -lxenon -lm

all: $(TARGET).elf32

$(TARGET).elf: $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

# elf32 is what XeLL loads
$(TARGET).elf32: $(TARGET).elf
	$(OBJCOPY) -O elf32-powerpc --adjust-vma 0x80000000 $< $@
	$(STRIP) $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET).elf $(TARGET).elf32

-include $(DEPS)

.PHONY: all clean
