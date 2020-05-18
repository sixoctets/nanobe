NANOBE_BASE = $(shell pwd)
export NANOBE_BASE

ifeq ($(BOARD), HiFive1)
	SOC = fe310
	INCLUDES = \
		-I board/HiFive1 \

else ifeq ($(BOARD), nrf5340pdk_nrf5340_cpuapp)
	SOC = nrf5340
	FLASH_START = 0x00000000
	FLASH_SIZE  = 0x00100000
	RAM_START   = 0x20000000
	RAM_SIZE    = 0x00080000

	ASFLAGS = \
		-mcpu=cortex-m33 \
		-mthumb

	CFLAGS = \
		-mcpu=cortex-m33 \
		-mthumb \
		-DNRF5340_XXAA_APPLICATION \
		-DDEBUG=1 \

	INCLUDES = \
		-I board/nrf5340pdk_nrf5340 \

else ifeq ($(BOARD), nrf5340pdk_nrf5340_cpunet)
	SOC = nrf5340
	FLASH_START = 0x01000000
	FLASH_SIZE  = 0x00040000
	RAM_START   = 0x21000000
	RAM_SIZE    = 0x00010000

	ASFLAGS = \
		-mcpu=cortex-m33+nodsp \
		-mthumb

	CFLAGS = \
		-mcpu=cortex-m33+nodsp \
		-mthumb \
		-DNRF5340_XXAA_NETWORK \
		-DDEBUG=1 \

	INCLUDES = \
		-I board/nrf5340pdk_nrf5340 \

else ifeq ($(BOARD), nrf52840dongle_nrf52840)
	SOC = nrf52840
	FLASH_START = 0x00001000
	FLASH_SIZE  = 0x000ff000
	RAM_START   = 0x20000000
	RAM_SIZE    = 0x00040000

	ASFLAGS = \
		-mcpu=cortex-m4 \
		-mthumb

	CFLAGS = \
		-mcpu=cortex-m4 \
		-mthumb \
		-DNRF52840_XXAA \
		-DDEBUG=1 \

	INCLUDES = \
		-I board/nrf52840dongle_nrf52840 \

else ifeq ($(BOARD), nrf51dk_nrf51822)
	SOC = nrf51822
	FLASH_START = 0x00000000
	FLASH_SIZE  = 0x00020000
	RAM_START   = 0x20000000
	RAM_SIZE    = 0x00004000

	ASFLAGS = \
		-mcpu=cortex-m0 \
		-mthumb

	CFLAGS = \
		-mcpu=cortex-m0 \
		-mthumb \
		-DNRF51 \
		-DDEBUG=1 \
		-DUART=8 \

	INCLUDES = \
		-I board/nrf51dk_nrf51822 \

endif

ifeq ($(SOC), fe310)
	ARCH = riscv
	SRCS_HAL_FE310 = \
		hal/fe310/gpio.c \

	SRCS_HAL = $(SRCS_HAL_FE310)

else ifeq ($(SOC), nrf5340)
	ARCH = arm
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

else ifeq ($(SOC), nrf52840)
	ARCH = arm
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

else ifeq ($(SOC), nrf51822)
	ARCH = arm
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

endif

ifeq ($(ARCH), riscv)
  ASMS_COMMON = \
	arch/riscv/startup.s \

  ASMS_NANOBE = \

else ifeq ($(ARCH), arm)
  ASMS_COMMON = \
	arch/arm/cortex_m/startup.s \

  ASMS_NANOBE = \
	arch/arm/cortex_m/nanobe.s \
	arch/arm/cortex_m/pendsv_ninject.s \

  INCLUDES += \
	-I ext/arm/cmsis/include \
	-I arch/arm/cortex_m \

  INCLUDES += \
	-I . \
	-I board/nrf5340pdk_nrf5340 \

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
	-I . \
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
