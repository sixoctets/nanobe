VERSION = $(shell git rev-parse --short HEAD)

AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size

ASFLAGS = \
	-mcpu=cortex-m4 \
	-mthumb

CFLAGS = \
	-mcpu=cortex-m4 \
	-mthumb \
	-Os \
	-nostdlib \
	-fdata-sections \
	-ffunction-sections \
	-flto \
	-g \
	-Wall \
	-Wextra \
	-Wcast-align \
	-DVERSION=\"$(VERSION)\" \
	-DNRF52840_XXAA \
	-DDEBUG=1 \
	-DUART=5 \
	$(INCLUDES)

OBJS = $(SRCS:.c=.o) $(ASMS:.s=.o)
DEPS = $(SRCS:.c=.d)

all : $(DEPS) $(OBJS)

$(SUBDIRS) :
	$(MAKE) -C $@

$(TARGETS) :
	$(CC) $(CFLAGS) -Wl,--gc-sections,--script=$(LDSCRIPT),-Map=$(basename $@).map -o $@ $^ -lgcc
	$(OBJDUMP) -D $@ > $(basename $@).lst
	$(OBJCOPY) -O ihex $@ $(basename $@).hex
	$(OBJCOPY) -O binary $@ $(basename $@).bin
	$(SIZE) $@

%.d : %.c
	$(CC) $(CFLAGS) -M -o $@ $<

clean:
	-for d in $(SUBDIRS); do ($(MAKE) -C $$d clean); done
	$(RM) $(DEPS) $(OBJS) $(TOBJS) $(TARGETS) $(TARGETS:.elf=.map) $(TARGETS:.elf=.hex) $(TARGETS:.elf=.bin) $(TARGETS:.elf=.lst)

.PHONY: clean $(SUBDIRS)

-include $(DEPS)
