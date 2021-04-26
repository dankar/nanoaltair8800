######################################
# target
######################################

BASE_BUILD_DIR=build
RELEASE_BUILD_DIR=$(BASE_BUILD_DIR)/release
DEBUG_BUILD_DIR=$(BASE_BUILD_DIR)/debug
PROFILE_BUILD_DIR=$(BASE_BUILD_DIR)/profile
TEST_BUILD_DIR=$(BASE_BUILD_DIR)/test

ifeq ($(OS), Windows_NT)
TEST_TARGET = tester.exe
else
TEST_TARGET = tester
endif
TARGET = nanoaltair
FULL_TEST_OUTPUT = auto_tests_full.log
BASIC_TEST_OUTPUT = auto_tests_basic.log

RELEASE_FOLDER=../releases

RELEASE_VERSION=$(shell git describe --tags)

COMMIT_HASH = $(shell git log --pretty=format:'%h' -n 1)$(shell git diff-index --quiet HEAD || echo "+mod")

RELEASE_BUILD_VERSION=$(RELEASE_VERSION)-$(COMMIT_HASH)
DEBUG_BUILD_VERSION=debug-$(COMMIT_HASH)

RELEASE_VERSION_DEFINE=-DBUILD_VERSION=\"$(RELEASE_BUILD_VERSION)\"
DEBUG_VERSION_DEFINE=-DBUILD_VERSION=\"$(DEBUG_BUILD_VERSION)\"

#######################################
# paths
#######################################
# Build path


######################################
# source
######################################
# C sources
C_STM32_SOURCES =  \
stm32cube/Core/Src/main.c \
stm32cube/Core/Src/stm32l4xx_it.c \
stm32cube/Core/Src/stm32l4xx_hal_msp.c \
stm32cube/FATFS/App/fatfs.c \
stm32cube/FATFS/Target/bsp_driver_sd.c \
stm32cube/FATFS/Target/sd_diskio.c \
stm32cube/FATFS/Target/fatfs_platform.c \
stm32cube/USB_DEVICE/App/usb_device.c \
stm32cube/USB_DEVICE/App/usbd_desc.c \
stm32cube/USB_DEVICE/App/usbd_cdc_if.c \
stm32cube/USB_DEVICE/Target/usbd_conf.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pcd.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pcd_ex.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usb.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_sdmmc.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sd.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sd_ex.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c \
stm32cube/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c \
stm32cube/Core/Src/system_stm32l4xx.c \
stm32cube/Middlewares/Third_Party/FatFs/src/diskio.c \
stm32cube/Middlewares/Third_Party/FatFs/src/ff.c \
stm32cube/Middlewares/Third_Party/FatFs/src/ff_gen_drv.c \
stm32cube/Middlewares/Third_Party/FatFs/src/option/syscall.c \
stm32cube/Middlewares/Third_Party/FatFs/src/option/ccsbcs.c \
stm32cube/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
stm32cube/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
stm32cube/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c \
stm32cube/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c

# ASM sources
ASM_STM32_SOURCES =  \
stm32cube/startup_stm32l496xx.s

C_MAIN_SOURCES = \
$(wildcard src/altair8800/*.c) \
$(wildcard src/sys/*.c) \
$(wildcard src/config/*.c) \
src/sys/arm/filesystem.c \
$(wildcard src/*.c)

C_TEST_SOURCES = \
$(wildcard src/altair8800/*.c) \
src/sys/log.c \
src/sys/buffer.c \
src/sys/pc/filesystem_pc.c \
$(wildcard test/*.c)


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
TEST_CC = gcc
RUN_FULL_TESTS = $(TEST_BUILD_DIR)/$(TEST_TARGET) level_one > $(FULL_TEST_OUTPUT)
RUN_BASIC_TESTS = $(TEST_BUILD_DIR)/$(TEST_TARGET) level_one > $(BASIC_TEST_OUTPUT)
DFUUTIL = dfu-util -a 0 -R -i 0 -s 0x08000000:leave -D

# mcu
MCU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32L496xx


# AS includes
AS_STM32_INCLUDES =

# C includes
C_STM32_INCLUDES =  \
-Istm32cube/FATFS/Target \
-Istm32cube/FATFS/App \
-Istm32cube/USB_DEVICE/App \
-Istm32cube/USB_DEVICE/Target \
-Istm32cube/Core/Inc \
-Istm32cube/Drivers/STM32L4xx_HAL_Driver/Inc \
-Istm32cube/Drivers/STM32L4xx_HAL_Driver/Inc/Legacy \
-Istm32cube/Middlewares/Third_Party/FatFs/src \
-Istm32cube/Middlewares/ST/STM32_USB_Device_Library/Core/Inc \
-Istm32cube/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc \
-Istm32cube/Drivers/CMSIS/Device/ST/STM32L4xx/Include \
-Istm32cube/Drivers/CMSIS/Include \
-Istm32cube/Drivers/CMSIS/Include

C_MAIN_INCLUDES = \
-Isrc


# compile gcc flags

# optimization
DEBUG_OPT = -Og -g -gdwarf-2 -DDEBUG_BUILD $(DEBUG_VERSION_DEFINE)
RELEASE_OPT = -Ofast $(RELEASE_VERSION_DEFINE)

BASE_ASFLAGS = $(MCU) $(AS_STM32_INCLUDES) -Wall -fdata-sections -ffunction-sections
RELEASE_ASFLAGS = $(BASE_ASFLAGS) $(RELEASE_OPT)
DEBUG_ASFLAGS = $(BASE_ASFLAGS) $(DEBUG_OPT)


BASE_CFLAGS = $(MCU) $(C_DEFS) $(C_STM32_INCLUDES) $(C_MAIN_INCLUDES) -Wall -fdata-sections -ffunction-sections -Werror -MMD -MP -MF"$(@:%.o=%.d)"

DEBUG_CFLAGS = $(BASE_CFLAGS) $(DEBUG_OPT) 
PROFILE_CFLAGS = $(DEBUG_CFLAGS) $(DEBUG_OPT) -finstrument-functions -finstrument-functions-exclude-file-list=stm32cube \
				-mpoke-function-name -DPROFILER_ENABLED=1
RELEASE_CFLAGS = $(BASE_CFLAGS) $(RELEASE_OPT) 
TEST_CFLAGS = -DTEST_BUILD -Werror -Wall -Ofast -MMD -MP -MF"$(@:%.o=%.d)" -DUINT=uint32_t $(RELEASE_VERSION_DEFINE)

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = stm32cube/STM32L496RETx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys
LIBDIR = 
RELEASE_LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(RELEASE_BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections  -u _printf_float
DEBUG_LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(DEBUG_BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections  -u _printf_float
PROFILE_LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(PROFILE_BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections  -u _printf_float

.PHONY = debug release all profile archive full_tests basic_tests
all: debug
debug: $(BASIC_TEST_OUTPUT) $(DEBUG_BUILD_DIR)/$(TARGET).elf $(DEBUG_BUILD_DIR)/$(TARGET).hex $(DEBUG_BUILD_DIR)/$(TARGET).bin
release: $(BASIC_TEST_OUTPUT) $(RELEASE_BUILD_DIR)/$(TARGET).elf $(RELEASE_BUILD_DIR)/$(TARGET).hex $(RELEASE_BUILD_DIR)/$(TARGET).bin
profile: $(BASIC_TEST_OUTPUT) $(PROFILE_BUILD_DIR)/$(TARGET).elf $(PROFILE_BUILD_DIR)/$(TARGET).hex $(PROFILE_BUILD_DIR)/$(TARGET).bin
archive: program.bat program.sh release $(FULL_TEST_OUTPUT) $(RELEASE_FOLDER)
	zip -j $(RELEASE_FOLDER)/nanoaltair-$(RELEASE_BUILD_VERSION).zip program.sh program.bat $(RELEASE_BUILD_DIR)/$(TARGET).bin $(RELEASE_BUILD_DIR)/$(TARGET).hex $(RELEASE_BUILD_DIR)/$(TARGET).elf $(FULL_TEST_OUTPUT) ../README.md
	-rm -f program.sh
	-rm -f program.bat
	cat $(FULL_TEST_OUTPUT)

full_tests: $(FULL_TEST_OUTPUT)
basic_tests: $(BASIC_TEST_OUTPUT)

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(notdir $(C_STM32_SOURCES:.c=.o))
vpath %.c $(sort $(dir $(C_STM32_SOURCES)))
# list of ASM program objects
OBJECTS += $(notdir $(ASM_STM32_SOURCES:.s=.o))
vpath %.s $(sort $(dir $(ASM_STM32_SOURCES)))

OBJECTS += $(notdir $(C_MAIN_SOURCES:.c=.o))
vpath %.c $(sort $(dir $(C_MAIN_SOURCES)))

TEST_OBJECTS = $(addprefix $(TEST_BUILD_DIR)/,$(notdir $(C_TEST_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_TEST_SOURCES)))

RELEASE_OBJECTS = $(addprefix $(RELEASE_BUILD_DIR)/, $(OBJECTS))
DEBUG_OBJECTS = $(addprefix $(DEBUG_BUILD_DIR)/, $(OBJECTS))
PROFILE_OBJECTS = $(addprefix $(PROFILE_BUILD_DIR)/, $(OBJECTS))

#######################################
# Compile targets
#######################################
$(TEST_BUILD_DIR)/%.o: %.c Makefile | $(TEST_BUILD_DIR)
	$(TEST_CC) -c $(VERSION_DEFINE) $(TEST_CFLAGS) $(C_MAIN_INCLUDES) $< -o $@

$(RELEASE_BUILD_DIR)/%.o: %.c Makefile | $(RELEASE_BUILD_DIR)
	$(CC) -c $(VERSION_DEFINE) $(RELEASE_CFLAGS) -Wa,-a,-ad,-alms=$(RELEASE_BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(DEBUG_BUILD_DIR)/%.o: %.c Makefile | $(DEBUG_BUILD_DIR)
	$(CC) -c $(VERSION_DEFINE) $(DEBUG_CFLAGS) -Wa,-a,-ad,-alms=$(DEBUG_BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(PROFILE_BUILD_DIR)/%.o: %.c Makefile | $(PROFILE_BUILD_DIR)
	$(CC) -c $(VERSION_DEFINE) $(PROFILE_CFLAGS) -Wa,-a,-ad,-alms=$(PROFILE_BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

#######################################
# Assembler targets
#######################################
$(RELEASE_BUILD_DIR)/%.o: %.s Makefile | $(RELEASE_BUILD_DIR)
	$(AS) -c $(RELEASE_ASFLAGS) $< -o $@

$(DEBUG_BUILD_DIR)/%.o: %.s Makefile | $(DEBUG_BUILD_DIR)
	$(AS) -c $(DEBUG_ASFLAGS) $< -o $@

$(PROFILE_BUILD_DIR)/%.o: %.s Makefile | $(PROFILE_BUILD_DIR)
	$(AS) -c $(DEBUG_ASFLAGS) $< -o $@

#######################################
# Link targets
#######################################
$(DEBUG_BUILD_DIR)/$(TARGET).elf: $(DEBUG_OBJECTS) Makefile
	echo $(TJO)
	$(CC) $(DEBUG_OBJECTS) $(DEBUG_LDFLAGS) -o $@
	$(SZ) $@

$(RELEASE_BUILD_DIR)/$(TARGET).elf: $(RELEASE_OBJECTS) Makefile
	$(CC) $(RELEASE_OBJECTS) $(RELEASE_LDFLAGS) -o $@
	$(SZ) $@

$(PROFILE_BUILD_DIR)/$(TARGET).elf: $(PROFILE_OBJECTS) Makefile
	$(CC) $(PROFILE_OBJECTS) $(PROFILE_LDFLAGS) -o $@
	$(SZ) $@

$(TEST_BUILD_DIR)/$(TEST_TARGET): $(TEST_OBJECTS) Makefile | $(TEST_BUILD_DIR)
	$(TEST_CC) -Ofast -Werror $(TEST_OBJECTS) -o $@

#######################################
# Generate hex/bin
#######################################
$(DEBUG_BUILD_DIR)/%.hex: $(DEBUG_BUILD_DIR)/%.elf | $(DEBUG_BUILD_DIR)
	$(HEX) $< $@

$(RELEASE_BUILD_DIR)/%.hex: $(RELEASE_BUILD_DIR)/%.elf | $(RELEASE_BUILD_DIR)
	$(HEX) $< $@

$(PROFILE_BUILD_DIR)/%.hex: $(PROFILE_BUILD_DIR)/%.elf | $(PROFILE_BUILD_DIR)
	$(HEX) $< $@

$(DEBUG_BUILD_DIR)/%.bin: $(DEBUG_BUILD_DIR)/%.elf | $(DEBUG_BUILD_DIR)
	$(BIN) $< $@

$(RELEASE_BUILD_DIR)/%.bin: $(RELEASE_BUILD_DIR)/%.elf | $(RELEASE_BUILD_DIR)
	$(BIN) $< $@

$(PROFILE_BUILD_DIR)/%.bin: $(PROFILE_BUILD_DIR)/%.elf | $(PROFILE_BUILD_DIR)
	$(BIN) $< $@

#######################################
# Generate folders
#######################################

$(BASE_BUILD_DIR):
	-mkdir $@

$(RELEASE_BUILD_DIR): | $(BASE_BUILD_DIR)
	-mkdir $@

$(DEBUG_BUILD_DIR): | $(BASE_BUILD_DIR)
	-mkdir $@

$(PROFILE_BUILD_DIR): | $(BASE_BUILD_DIR)
	-mkdir $@

$(TEST_BUILD_DIR): | $(BASE_BUILD_DIR)
	-mkdir $@

$(RELEASE_FOLDER):
	-mkdir $@

$(FULL_TEST_OUTPUT): $(TEST_BUILD_DIR)/$(TEST_TARGET)
	$(RUN_FULL_TESTS)

$(BASIC_TEST_OUTPUT): $(TEST_BUILD_DIR)/$(TEST_TARGET)
	$(RUN_BASIC_TESTS)

clean:
	-rm -fR program.sh
	-rm -fR program.bat
	-rm -fR $(BASE_BUILD_DIR)
	-rm -fR $(BASIC_TEST_OUTPUT)
	-rm -fR $(FULL_TEST_OUTPUT)

program_debug: $(BASIC_TEST_OUTPUT) $(DEBUG_BUILD_DIR)/$(TARGET).bin
	$(DFUUTIL) $(DEBUG_BUILD_DIR)/$(TARGET).bin

program_release: $(BASIC_TEST_OUTPUT) $(RELEASE_BUILD_DIR)/$(TARGET).bin
	$(DFUUTIL) $(RELEASE_BUILD_DIR)/$(TARGET).bin

program_profile: $(BASIC_TEST_OUTPUT) $(PROFILE_BUILD_DIR)/$(TARGET).bin
	$(DFUUTIL) $(PROFILE_BUILD_DIR)/$(TARGET).bin

program.sh:
	echo "$(DFUUTIL) $(TARGET).bin" > program.sh
	chmod +x program.sh

program.bat: program.sh
	cat program.sh > program.bat

-include $(wildcard $(DEBUG_BUILD_DIR)/*.d)
-include $(wildcard $(RELEASE_BUILD_DIR)/*.d)
-include $(wildcard $(PROFILE_BUILD_DIR)/*.d)
-include $(wildcard $(TEST_BUILD_DIR)/*.d)
