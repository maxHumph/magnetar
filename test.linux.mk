CC=clang
BUILD_DIR=bin
OBJ_DIR=$(BUILD_DIR)/obj

ASSEMBLY=test
EXTENSION=
COMPILE_FLAGS=-g -fdeclspec -fPIC 
CFLAGS_EXTRA=-Wall -Wextra -Werror
INCLUDE_FLAGS=-Iengine/src -I$(VULKAN_SDK)/include
LINKER_FLAGS=-L./$(BUILD_DIR)/ -lengine -Wl,-rpath,.
DEFINES=-D_DEBUG -DMIMPORT

SRC_FILES=$(shell find $(ASSEMBLY) -name *.c)
DIRS=$(shell find $(ASSEMBLY) -type d)
OBJ_FILES=$(SRC_FILES:%=$(OBJ_DIR)/%.o)

all: scaffold compile link

.PHONY: scaffold
scaffold:
	@echo Creating folder structure...
	@mkdir -p $(addprefix $(OBJ_DIR)/,$(DIRS))
	@echo Finished creating folder structure.

.PHONY: link
link: scaffold $(OBJ_FILES)
	@echo Linking $(ASSEMBLY)...
	@$(CC) $(OBJ_FILES) -o $(BUILD_DIR)/$(ASSEMBLY)$(EXTENSION) $(LINKER_FLAGS)
	@echo Finished linking $(ASSEMBLY).

.PHONY: compile
	@echo Compiling $(ASSEMBLY)...

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)/$(ASSEMBLY)
	rm -rf $(BUILD_DIR)/$(ASSEMBLY)

$(OBJ_DIR)/%.c.o: %.c
	@echo $<...
	@$(CC) $< $(COMPILE_FLAGS) -c -o $@ $(DEFINES) $(INCLUDE_FLAGS)
