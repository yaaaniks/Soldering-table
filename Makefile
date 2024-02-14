PROJECT 				= table
CORE_DIR 				= ./core
DEV_DIR 				= $(CORE_DIR)/device
ARM_DIR 				= $(CORE_DIR)/arm
SYS_DIR 				= $(CORE_DIR)/system

INC_DIRS 				= $(DEV_DIR) $(ARM_DIR) $(SYS_DIR) inc src common
SRC_DIRS 				= $(DEV_DIR) $(ARM_DIR) $(SYS_DIR) inc src common

BINUTILS_ROOT           := /usr

vpath %.h	$(INC_DIRS)
vpath %.c 	$(SRC_DIRS)
vpath %.cpp $(SRC_DIRS)

find_includes_in_dir = $(shell find $(1) -name "*.h" | sed 's|/[^/]*$$||' | sort -u)

include Makefile.include # specific rules for stm32g031
include rules.mk # generic rules