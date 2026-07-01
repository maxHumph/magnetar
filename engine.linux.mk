CC=clang
BUILD_DIR=bin
OBJ_DIR=$(BUILD_DIR)/obj

ASSEMBLY=engine
EXTENSION=.so
DEPFLAGS=-MMD -MP
COMPILE_FLAGS=-g -fdeclspec -fPIC $(DEPFLAGS)
CFLAGS_EXTRA=-Wall -Wextra -Werror
INCLUDE_FLAGS=-Iengine/src -I$(VULKAN_SDK)/include
LINKER_FLAGS=-shared -lvulkan -lxcb -lX11 -lX11-xcb -lxkbcommon -lm -L$(VULKAN_SDK)/lib -L/usr/X11R6/lib $(DEPFLAGS)
DEFINES=-D_DEBUG -DMEXPORT

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
	@$(CC) $(OBJ_FILES) -o $(BUILD_DIR)/lib$(ASSEMBLY)$(EXTENSION) $(LINKER_FLAGS)
	@echo Finished linking $(ASSEMBLY).

.PHONY: compile
	@echo Compiling $(ASSEMBLY)...

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)/$(ASSEMBLY)
	rm -rf $(BUILD_DIR)/lib$(ASSEMBLY)$(EXTENSION)

$(OBJ_DIR)/%.c.o: %.c
	@echo $<...
	@mkdir -p $(dir $@)
	@$(CC) $< $(COMPILE_FLAGS) -c -o $@ $(DEFINES) $(INCLUDE_FLAGS)

-include $(OBJ_FILES:.o=.d)
