CC=clang
BUILD_DIR=bin
OBJ_DIR=$(BUILD_DIR)/obj

ASSEMBLY=engine
EXTENSION=.so

ifdef VULKAN_SDK
	VK_IFLAGS=-I$(VULKAN_SDK)/include
	VK_LFLAGS=-L$(VULKAN_SDK)/lib -lvulkan
else
	VK_IFLAGS=$(shell pkg-config --cflags vulkan vulkan-utility-libraries)
	VK_LFLAGS=$(shell pkg-config --libs vulkan)
endif

DEPFLAGS=-MMD -MP
COMPILE_FLAGS=-g -fdeclspec -fPIC $(DEPFLAGS)
CFLAGS_EXTRA=-Wall -Wextra -Werror
INCLUDE_FLAGS=-Iengine/src $(VK_IFLAGS)
LINKER_FLAGS=-shared -lxcb -lX11 -lX11-xcb -lxkbcommon -lm $(VK_LFLAGS) -L/usr/X11R6/lib $(DEPFLAGS)
DEFINES=-D_DEBUG -DMEXPORT

SRC_FILES=$(shell find $(ASSEMBLY) -name *.c)
DIRS=$(shell find $(ASSEMBLY) -type d)
OBJ_FILES=$(SRC_FILES:%=$(OBJ_DIR)/%.o)

all: shaders scaffold compile link

.PHONY: shaders
shaders:
	@./compile-shader.sh engine/src/renderer/vulkan/shaders/basic
	@./compile-shader.sh engine/src/renderer/vulkan/shaders/basic_ui

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
