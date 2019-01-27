SUBDIRS = \
	arch/arm/cortex_m \
	soc/nrf5 \
	hal/nrf5 \
	nanobe \
	util \
	app \

ASMS_COMMON = \
	arch/arm/cortex_m/startup.s \

ASMS_NANOBE = \
	$(ASMS_COMMON) \
	arch/arm/cortex_m/nanobe.s \
	arch/arm/cortex_m/pendsv_ninject.s \

SRCS_NANOBE = \
	nanobe/isr_table.c \
	nanobe/nanobe_sched.c \

ASMS_SOC_NRF5 = \
	soc/nrf5/soc.s \

SRCS_SOC_NRF5 = \
	soc/nrf5/soc_c.c \

SRCS_HAL_NRF5 = \
	hal/nrf5/uart.c \

SRCS_UTIL = \
	util/util.c \

ASMS_APP_NANOBE = \
	$(ASMS_NANOBE) \
	$(ASMS_SOC_NRF5) \

SRCS_APP_NANOBE = \
	$(SRCS_NANOBE) \
	$(SRCS_SOC_NRF5) \
	$(SRCS_HAL_NRF5) \
	$(SRCS_UTIL) \
	app/app_nanobe.c \

LDSCRIPT = arch/arm/cortex_m/link.lds

TARGETS = \
	app/app_nanobe.elf \

all : $(SUBDIRS) $(TARGETS)

app/app_nanobe.elf : $(ASMS_APP_NANOBE:.s=.o)
app/app_nanobe.elf : $(SRCS_APP_NANOBE:.c=.o)

include ./makefile.inc
