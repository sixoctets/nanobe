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

ASMS_APP_PROFILE = \
	$(ASMS_NANOBE) \
	$(ASMS_SOC_NRF5) \

SRCS_APP_PROFILE = \
	$(SRCS_NANOBE) \
	$(SRCS_SOC_NRF5) \
	$(SRCS_HAL_NRF5) \
	$(SRCS_UTIL) \
	app/app_profile.c \

ASMS_APP_SENSOR = \
	$(ASMS_NANOBE) \
	$(ASMS_SOC_NRF5) \

SRCS_APP_SENSOR = \
	$(SRCS_NANOBE) \
	$(SRCS_SOC_NRF5) \
	$(SRCS_HAL_NRF5) \
	$(SRCS_UTIL) \
	app/app_sensor.c \

ASMS_APP_ULTRASOUND = \
	$(ASMS_NANOBE) \
	$(ASMS_SOC_NRF5) \

SRCS_APP_ULTRASOUND = \
	$(SRCS_NANOBE) \
	$(SRCS_SOC_NRF5) \
	$(SRCS_HAL_NRF5) \
	$(SRCS_UTIL) \
	app/app_ultrasound.c \

LDSCRIPT = arch/arm/cortex_m/link.lds

TARGETS = \
	app/app_nanobe.elf \
	app/app_profile.elf \
	app/app_sensor.elf \
	app/app_ultrasound.elf \

all : $(SUBDIRS) $(TARGETS)

app/app_nanobe.elf : $(ASMS_APP_NANOBE:.s=.o)
app/app_nanobe.elf : $(SRCS_APP_NANOBE:.c=.o)

app/app_profile.elf : $(ASMS_APP_PROFILE:.s=.o)
app/app_profile.elf : $(SRCS_APP_PROFILE:.c=.o)

app/app_sensor.elf : $(ASMS_APP_SENSOR:.s=.o)
app/app_sensor.elf : $(SRCS_APP_SENSOR:.c=.o)

app/app_ultrasound.elf : $(ASMS_APP_ULTRASOUND:.s=.o)
app/app_ultrasound.elf : $(SRCS_APP_ULTRASOUND:.c=.o)

include ./makefile.inc
