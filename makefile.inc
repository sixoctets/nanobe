ifeq ($(ARCH), riscv)
	include $(NANOBE_BASE)/makefile.riscv
else ifeq ($(ARCH), arm)
	include $(NANOBE_BASE)/makefile.arm
endif

ASFLAGS += \

CFLAGS += \
	-Os \
	-nostdlib \
	-fdata-sections \
	-ffunction-sections \
	-flto \
	-g \
	-Wall \
	-Wextra \
	-Wcast-align \
	-I ./ \
	$(INCLUDES) \
	-DVERSION=\"$(VERSION)\" \

OBJS += $(SRCS:.c=.o) $(ASMS:.s=.o)
DEPS += $(SRCS:.c=.d)

VERSION = $(shell git rev-parse --short HEAD)

all : $(TARGETS)

info :
	@echo NANOBE_BASE = $(NANOBE_BASE)
	@echo TARGETS = $(TARGETS)
	@echo ASFLAGS = $(ASFLAGS)
	@echo CFLAGS = $(CFLAGS)
	@echo OBJS = $(OBJS)
	@echo DEPS = $(DEPS)

$(TARGETS) :
	$(CC) $(CFLAGS) -Wl,--gc-sections,--script=$(LDSCRIPT),-Map=$(basename $@).map -o $@ $^ -lgcc
	$(OBJDUMP) -D $@ > $(basename $@).lst
	$(OBJCOPY) -O ihex $@ $(basename $@).hex
	$(OBJCOPY) -O binary $@ $(basename $@).bin
	$(SIZE) $@

%.d : %.c
	$(CC) $(CFLAGS) -M -o $@ $<

clean :
	$(RM) $(DEPS) $(OBJS) $(TARGETS) $(TARGETS:.elf=.map) $(TARGETS:.elf=.hex) $(TARGETS:.elf=.bin) $(TARGETS:.elf=.lst)

.PHONY : all info clean

-include $(DEPS)
