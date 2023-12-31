#########################################################################################################################################
# Basic Makefile 
# 	- It uses GCC's autodependency feature to maintain a list of dependencies for each source file in the build directory -- this means
# 		it will recompile only the files that need to be recompiled, automatically.
# 	- The majority of the makefile is a defined variable that is evaluated with a set of parameters that can either be set at the bottom
# 		of this file or passed in via the command line. Evaluation happens at the end of the file, with defaults defined just before.
#########################################################################################################################################

# Use Bash sdf
SHELL = /bin/sh
PROJECT = main
# Functions
find_includes_in_dir = $(shell find $(1) -name "*.h" | sed 's|/[^/]*$$||' | sort -u)

# ---------------------------------------------------------------------
# Toolchain Configuration
# ---------------------------------------------------------------------
BINUTILS_ROOT           ?= /usr/
TOOLCHAIN               := arm-none-eabi
CC                      := $(BINUTILS_ROOT)/bin/$(TOOLCHAIN)-gcc
CXX                     := $(BINUTILS_ROOT)/bin/$(TOOLCHAIN)-g++
AS                      := $(BINUTILS_ROOT)/bin/$(TOOLCHAIN)-as
OBJCOPY					:= $(BINUTILS_ROOT)/bin/$(TOOLCHAIN)-objcopy
OBJDUMP 				:= $(BINUTILS_ROOT)/bin/$(TOOLCHAIN)-objdump
SIZE 					:= $(BINUTILS_ROOT)/bin/$(TOOLCHAIN)-size
C_STANDARD				:= -std=gnu11
CXX_STANDARD 			:= -std=gnu++11

# -----------------------------------------------------------------------------------------------------------------
# Defined Symbols
# -----------------------------------------------------------------------------------------------------------------
DEFS 					:= -DSTM32G031xx -DARM_MATH_CM0PLUS -D__FPU_PRESENT=1U -DHSI_VALUE=16000000

# ---------------------------------------------------------------------------------------------------------------------------------------
# Compiler & Linker Flags
# ---------------------------------------------------------------------------------------------------------------------------------------
# Flags sent to all tools in the Toolchain 
TOOLCHAIN_SETTINGS 		:= -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft #-mfpu=fpv4-sp-d16 #cortex-m0plus.small-multiply
TOOLCHAIN_SETTINGS 		+= -fmessage-length=0 -ffunction-sections -fdata-sections

# C Compiler -- Warnings 
CFLAGS 					+= $(TOOLCHAIN_SETTINGS) $(DEFS) $(addprefix -I, $(INC_DIRS))
CFLAGS                  += -Wall
CFLAGS 					+= -Wextra
CFLAGS 					+= -Wfatal-errors
CFLAGS 					+= -Wpacked
CFLAGS 					+= -Winline
CFLAGS 					+= -Wfloat-equal
CFLAGS 					+= -Wconversion
CFLAGS 					+= -Wlogical-op
CFLAGS 					+= -Wpointer-arith
CFLAGS 					+= -Wdisabled-optimization
CFLAGS                	+= -Wno-unused-parameter
CFLAGS                  += -Wa,-alh=$(@:.o=.lst)

# C++ Compiler -- Required & Optimization Flags
CXXFLAGS                += $(CFLAGS)
CXXFLAGS 				+= -fabi-version=0
CXXFLAGS                += -fno-rtti
CXXFLAGS                += -fno-exceptions
CXXFLAGS				+= -fno-use-cxa-atexit
CXXFLAGS 				+= -fno-threadsafe-statics

# C++ -- Warnings
CXXFLAGS 				+= -Weffc++
CXXFLAGS 				+= -Wfloat-equal
CXXFLAGS 				+= -Wsign-promo
CXXFLAGS 				+= -Wzero-as-null-pointer-constant
CXXFLAGS 				+= -Wmissing-declarations 
CXXFLAGS 				+= -Woverloaded-virtual
CXXFLAGS 				+= -Wsuggest-final-types
CXXFLAGS 				+= -Wsuggest-final-methods
CXXFLAGS 				+= -Wsuggest-override
CXXFLAGS 				+= -Wsuggest-attribute=pure
CXXFLAGS 				+= -Wsuggest-attribute=const
CXXFLAGS 				+= -Wsuggest-attribute=noreturn
CXXFLAGS 				+= -Wsuggest-attribute=format
CXXFLAGS 				+= -Wmissing-format-attribute
CXXFLAGS 				+= -Wold-style-cast
CXXFLAGS 				+= -Wshadow
CXXFLAGS 				+= -Wuseless-cast
CXXFLAGS 				+= -Wctor-dtor-privacy
CXXFLAGS 				+= -Wstrict-null-sentinel

# Linker
LDFLAGS 				+= $(TOOLCHAIN_SETTINGS) $(DEFS) -Xlinker --gc-sections --specs=nano.specs -lm

# -------------------------------------------------------------
# Build Type Modifiers
# -------------------------------------------------------------
# Debug
DEFS_DEBUG 				+= -DDEBUG
CFLAGS_DEBUG            += -ggdb -g3 -Og
LDFLAGS_DEBUG			+= --specs=rdimon.specs -Og 

# Release
CFLAGS_RELEASE			+= -Os
LDFLAGS_RELEASE 		+= --specs=nosys.specs

#########################################################################################################################################
# RULE DEFINITIONS -- This section is generic
#########################################################################################################################################

# =======================================================================================================================================
# Build Configuration Rule 
# - Generate build config using Product Root Directory ($1), Build Type ("Debug" or "Release") ($2)
# =======================================================================================================================================
define CONFIG_RULE
BUILD_DIR 				:= build/$2
OBJ_DIR 				:= $$(BUILD_DIR)/obj
INC_DIRS 				:= cmsis/core cmsis/device Core cpu src inc common
SRC_DIRS 				:= cmsis/core cmsis/device Core cpu src inc common
HEADERS 				:= $$(foreach dir, $$(SRC_DIRS), $$(shell find $$(dir) -name "*.h"))
ASM_SRC 				:= $$(foreach dir, $$(SRC_DIRS), $$(shell find $$(dir) -name "*.s"))
C_SRC					:= $$(foreach dir, $$(SRC_DIRS), $$(shell find $$(dir) -name "*.c" -not -name "src/main.c"))
CXX_SRC					:= $$(foreach dir, $$(SRC_DIRS), $$(shell find $$(dir) -name "*.cpp"))
OBJECTS                 := $$(addprefix $$(OBJ_DIR)/, $$(C_SRC:.c=.o) $$(CXX_SRC:.cpp=.o) $$(ASM_SRC:.s=.o))
LDSCRIPTS				:= $$(addprefix -T, $$(foreach dir, $$(SRC_DIRS), $$(shell find $$(dir) -name "*.ld")))
DIRS 					:= $$(BUILD_DIR) $$(sort $$(dir $$(OBJECTS)))
AUTODEPS 				:= $$(OBJECTS:.o=.d)


ifeq ($2, Release)
	DEFS 	+= $$(DEFS_RELEASE)
	CFLAGS 	+= $$(CFLAGS_RELEASE)
	LDFLAGS += $$(LDFLAGS_RELEASE)
else 
	DEFS 	+= $$(DEFS_DEBUG)
	CFLAGS 	+= $$(CFLAGS_DEBUG)
	LDFLAGS += $$(LDFLAGS_DEBUG)
endif

endef 
# =======================================================================================================================================
# End CONFIG_RULE
# =======================================================================================================================================


# =======================================================================================================================================
# Build Target Rule 
# - Generate build config using Product Name ($1), Product Root Directory ($2), Build Type ("Debug" or "Release") ($3)
# =======================================================================================================================================
define BUILD_TARGET_RULE
$(eval $(call CONFIG_RULE,$2,$3))

all : $$(BUILD_DIR)/$1.elf $$(BUILD_DIR)/$1.hex

# Tool Invocations
$$(BUILD_DIR)/$1.elf : $$(OBJECTS) | $$(BUILD_DIR)
	@echo ' '
	@echo 'Building $$(@)'
	@echo 'Invoking: Cross ARM C++ Linker'
	$$(CXX) \
		-Xlinker -Map=$$(patsubst %.elf,%.map,$$(@)) \
		$$(LDFLAGS) \
		$$(LDSCRIPTS) \
		-o $$(@) $$(OBJECTS)
	@echo 'Finished building: $$@'
	@echo ' '
	@echo $$(build)

%.hex : %.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	$$(OBJCOPY) -O ihex $$< $$(@) 
	$$(OBJCOPY) -O binary $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).bin
	$$(OBJDUMP) -St $(BUILD_DIR)/$(PROJECT).elf >$(BUILD_DIR)/$(PROJECT).lst
	@echo 'Finished building: $$@'
	@echo ' '
	@echo 'Invoking: Cross ARM GNU Print Size'
	$$(SIZE) --format=berkeley $$<
	@echo 'Finished building: $$@'
	@echo ' '

$$(OBJECTS) : | $$(DIRS)

$$(DIRS) : 
	@echo Creating $$(@)
	@mkdir -p $$(@)

$$(OBJ_DIR)/%.o : %.c
	@echo Compiling $$(<F)
	@$$(CC) $$(C_STANDARD) $$(CFLAGS) -c -MMD -MP $$< -o $$(@)

$$(OBJ_DIR)/%.o : %.cpp
	@echo Compiling $$(<F)
	@$$(CXX) $$(CXX_STANDARD) $$(CXXFLAGS) -c -MMD -MP $$< -o $$(@)

$$(OBJ_DIR)/%.o : %.s
	@echo Assembling $$(<F)
	@$$(AS) $$(ASFLAGS) $$< -o $$(@)




clean :
	@rm -rf build

.PHONY : clean all

# include by auto dependencies
-include $$(AUTODEPS)

endef
# =======================================================================================================================================
# End BUILD_TARGET_RULE
# =======================================================================================================================================
#########################################################################################################################################
#########################################################################################################################################

# Build Type
ifeq ($(build), Debug)
	BUILD_TYPE := Debug
else
	BUILD_TYPE := Release
endif


# Defaults
PRODUCT ?= main
PRODUCT_DIR ?= project
BUILD_TYPE ?= Debug
SRC_DIRS ?= $(PRODUCT_DIR)

# Evaluate Rules Defined Above
$(eval $(call BUILD_TARGET_RULE,$(PRODUCT),$(PRODUCT_DIR),$(BUILD_TYPE)))
