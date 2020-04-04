NANOBE_BASE = $(shell pwd)
export NANOBE_BASE

ifeq ($(ARCH), riscv)
  ASMS_COMMON = \
	arch/riscv/startup.s \

  ASMS_NANOBE = \

  SRCS_HAL_FE310 = \
	hal/fe310/gpio.c \

  SRCS_HAL = $(SRCS_HAL_FE310)

  INCLUDES += \
	-I . \
	-I board/HiFive1 \

else ifeq ($(ARCH), arm)
  ASMS_COMMON = \
	arch/arm/cortex_m/startup.s \

  ASMS_NANOBE = \
	arch/arm/cortex_m/nanobe.s \
	arch/arm/cortex_m/pendsv_ninject.s \

  INCLUDES += \
	-I ext/arm/cmsis/include \
	-I arch/arm/cortex_m \

  ASMS_SOC_NRF5 = \
	soc/nrf5/soc.s \

  SRCS_SOC_NRF5 = \
	soc/nrf5/soc_c.c \

  SRCS_HAL_NRF5 = \
	hal/nrf5/gpio.c \
	hal/nrf5/clock.c \
	hal/nrf5/uart.c \

  ASMS_SOC = $(ASMS_SOC_NRF5)
  SRCS_SOC = $(SRCS_SOC_NRF5)
  SRCS_HAL = $(SRCS_HAL_NRF5)

  INCLUDES += \
	-I . \
	-I board/nRF52840Dongle_nRF52840 \

endif

SRCS_NANOBE = \
	nanobe/isr_table.c \
	nanobe/nanobe_sched.c \

SRCS_UTIL = \
	util/util.c \

ASMS_APP_METAL = \
	$(ASMS_COMMON) \
	$(ASMS_SOC) \

SRCS_APP_METAL = \
	$(SRCS_SOC) \
	$(SRCS_HAL) \
	app/app_metal.c \

OBJS_APP_METAL = $(ASMS_APP_METAL:.s=.o) $(SRCS_APP_METAL:.c=.o)
ASMS += $(ASMS_APP_METAL)
SRCS += $(SRCS_APP_METAL)
TARGETS += app/app_metal.elf

ifeq ($(ARCH), arm)
  ASMS_APP_PROFILE = \
	$(ASMS_COMMON) \
	$(ASMS_NANOBE) \
	$(ASMS_SOC_NRF5) \

  SRCS_APP_PROFILE = \
	$(SRCS_NANOBE) \
	$(SRCS_SOC_NRF5) \
	$(SRCS_HAL_NRF5) \
	$(SRCS_UTIL) \
	app/app_profile.c \

  INCLUDES += \
	-I ext/nordic/include \
	-I soc/nrf5 \
	-I nanobe \

  OBJS_APP_PROFILE = $(ASMS_APP_PROFILE:.s=.o) $(SRCS_APP_PROFILE:.c=.o)
  ASMS += $(ASMS_APP_PROFILE)
  SRCS += $(SRCS_APP_PROFILE)
  TARGETS += app/app_profile.elf
endif

all :

app/app_metal.elf : $(OBJS_APP_METAL)

app/app_profile.elf : $(OBJS_APP_PROFILE)

include makefile.inc
