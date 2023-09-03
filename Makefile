PROJECT=DIYtable

# FIXME: debugging is not allow correctly
MCU=STM32G031xx

INCLUDE_DIR=./inc
SOURCE_DIR=./src
COMMON_DIR=./common
CORE_DIR=./Core
CMSIS_DIR=./cmsis
CPU_DIR=./cpu
BUILD_DIR=build

# DEF=-DSTM32F401xC
OPT = -Og
# OPT=-O3 -g0 -flto
SRC=$(SOURCE_DIR)/main.c 
SRC+=$(SOURCE_DIR)/system_stm32g0xx.c
SRC+=$(COMMON_DIR)/fsm.c 
SRC+=$(CORE_DIR)/timer.c
SRC+=$(CORE_DIR)/i2c.c
SRC+=$(CORE_DIR)/adc.c
SRC+=$(CORE_DIR)/exti.c
SRC+=$(CORE_DIR)/rcc.c
SRC+=$(CORE_DIR)/io.c
SRC+=$(COMMON_DIR)/ssd1306.c
SRC+=$(COMMON_DIR)/ssd1306_fonts.c
SRC+=$(COMMON_DIR)/ssd1306_tests.c
SRC+=$(COMMON_DIR)/tempControl.c

# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES_X:.S=.o)))
vpath %.S $(sort $(dir $(ASM_SOURCES_X)))


# SRC_FILES = $(wildcard $(SOURCE_DIR)*.c) $(wildcard $(SOURCE_DIR)*/*.c)
# ASM_FILES = $(wildcard $(CPU_DIR)*.s) $(wildcard $(CPU_DIR)*/*.s)
# CORE_FILES = $(wildcard $(CORE_DIR)*.c) $(wildcard $(CORE_DIR)*/*.c)
###################################################

CSTANDARD = -std=c11

# External libraries
# LIBRARIES = lib

CMSIS = $(CMSIS_DIR)/device
CORE = $(CMSIS_DIR)/core
###################################################

# Location of linker script and startup file
LINKER_SCRIPT = $(CPU_DIR)/stm32g031x8.ld
STARTUP_SCRIPT = $(CPU_DIR)/startup_stm32g031xx.s
###################################################

# vpath %.a ./cpu
VPATH +=$(SOURCE_DIR) $(CPU_DIR) $(CORE_DIR) $(COMMON_DIR)

CC=arm-none-eabi-gcc
GDB=arm-none-eabi-gdb
OBJCOPY=arm-none-eabi-objcopy
OBJDUMP=arm-none-eabi-objdump
SIZE=arm-none-eabi-size

# CPU = cortex-m0plus

# mcu
# MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)
ARCH_FLAGS = -mthumb -mcpu=cortex-m0plus
OOCD_INTERFACE = stlink
OOCD_TARGET = stm32g0x
# 
BUILD_DIR ?= bin
OPT ?= -Os
CSTD ?= -std=c11
# 
TGT_CPPFLAGS += -MD
TGT_CPPFLAGS += -Wall -Wundef $(INCLUDES)
TGT_CPPFLAGS += $(INCLUDES) $(OPENCM3_DEFS)

TGT_CFLAGS += $(OPT) $(CSTD) -ggdb3
TGT_CFLAGS += $(ARCH_FLAGS)
TGT_CFLAGS += -fno-common
TGT_CFLAGS += -ffunction-sections -fdata-sections
TGT_CFLAGS += -Wextra -Wshadow -Wno-unused-variable -Wimplicit-function-declaration
TGT_CFLAGS += -Wredundant-decls -Wstrict-prototypes -Wmissing-prototypes

TGT_CXXFLAGS += $(OPT) $(CXXSTD) -ggdb3
TGT_CXXFLAGS += $(ARCH_FLAGS)
TGT_CXXFLAGS += -fno-common
TGT_CXXFLAGS += -ffunction-sections -fdata-sections
TGT_CXXFLAGS += -Wextra -Wshadow -Wredundant-decls  -Weffc++ -nostartfiles

TGT_LDFLAGS += $(ARCH_FLAGS)
TGT_LDFLAGS += -specs=nano.specs
TGT_LDFLAGS += -Wl,--gc-sections
# OPTIONAL
#TGT_LDFLAGS += -Wl,-Map=$(PROJECT).map
ifeq ($(V),99)
TGT_LDFLAGS += -Wl,--print-gc-sections
endif

# Linker script generator fills this in for us.
ifeq (,$(DEVICE))
# LDLIBS += -l$(OPENCM3_LIB)
endif
# nosys is only in newer gcc-arm-embedded...
#LDLIBS += -specs=nosys.specs
LDLIBS += -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group

# CFLAGS  = -Wall -Wextra -Warray-bounds
# CFLAGS += -mcpu=cortex-m0plus -mthumb -mlittle-endian -mthumb-interwork
# CFLAGS += -mfpu=fpv4-sp-d16
# CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += $(TGT_CFLAGS)

# CFLAGS  = -Wall -g3 $(CSTANDARD) # -Os $(OPT)
# CFLAGS += -mlittle-endian -mcpu=cortex-m4 -march=armv7e-m -mthumb
# CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
# CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -D$(MCU)
CFLAGS += -DDEBUG
CFLAGS += -I$(INCLUDE_DIR)
CFLAGS += -I$(CORE_DIR)
CFLAGS += -I$(SOURCE_DIR)
CFLAGS += -I$(CMSIS)
CFLAGS += -I$(CORE)
CFLAGS += -I$(COMMON_DIR)
# CFLAGS += -g -gdwarf-2


# LDFLAGS += -Wl,--gc-sections -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map -ffreestanding -Xlinker
LDFLAGS+=-T$(LINKER_SCRIPT) --specs=rdimon.specs -lc -lrdimon $(LDLIBS) $(TGT_LDFLAGS) --specs=nosys.specs
# CFLAG   = -mcpu=$(CPU) -mthumb -Wall -fdump-rtl-expand -specs=nano.specs --specs=rdimon.specs   -Wl,--start-group -lgcc -lc -lm -lrdimon -Wl,--end-group
# LDFLAG  = -mcpu=$(CPU) -T ./stm32_flash.ld -specs=nano.specs --specs=rdimon.specs   -Wl,--start-group -lgcc -lc -lm -lrdimon -Wl,--end-group

###################################################

ROOT=$(shell pwd)

OBJS = $(patsubst $(SOURCE_DIR)/%.c,$(BUILD_DIR)/objs/%.o,$(SRC))
DEPS = $(addprefix $(BUILD_DIR)/deps/,$(SRC:.c=.d))

###################################################

all: proj

-include $(DEPS)

proj: $(BUILD_DIR) $(BUILD_DIR)/$(PROJECT).elf

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/deps $(BUILD_DIR)/objs

$(BUILD_DIR)/objs/%.o : src/%.c $(BUILD_DIR) $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< -MMD -MF $(BUILD_DIR)/deps/$(*F).d

$(BUILD_DIR)/$(PROJECT).elf: $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(STARTUP_SCRIPT) 
	$(OBJCOPY) -O ihex $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).hex
	$(OBJCOPY) -O binary $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).bin
	$(OBJDUMP) -St $(BUILD_DIR)/$(PROJECT).elf >$(BUILD_DIR)/$(PROJECT).lst
	$(SIZE) $(BUILD_DIR)/$(PROJECT).elf

$(BUILD_DIR)/%.o: %.c
	@printf "  CC\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(CC) $(TGT_CFLAGS) $(CFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $@ -c $<

$(BUILD_DIR)/%.o: %.cxx
	@printf "  CXX\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(CC) $(TGT_CXXFLAGS) $(CXXFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $@ -c $<

program: all
	@sleep 1
	st-flash write `pwd`/$(BUILD_DIR)/$(PROJECT).bin 0x08000000

debug: program
	$(GDB) -x extra/gdb_cmds $(PROJECT).elf

clean:
	find ./ -name '*~' | xargs rm -f	
	rm -rf $(BUILD_DIR)

reallyclean: clean
	$(MAKE) -C clean

.PHONY: all proj program debug flash clean reallyclean