CC ?= clang

SRC_PATH = src
TEST_PATH = test
BUILD_PATH = build
BIN_PATH = $(BUILD_PATH)/bin

BIN_NAME = $(shell basename $$(pwd))
TEST_BIN_NAME = $(BIN_NAME)-tests

SRC_EXT = c

SOURCES = $(shell find $(SRC_PATH) -name '*.$(SRC_EXT)' | sort -k 1nr | cut -f2-)
OBJECTS = $(SOURCES:$(SRC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o)

OBJECTS_TO_TEST = $(shell echo $(OBJECTS) | sed -E 's/\$(BUILD_PATH)\/main.o//g')
TEST_SOURCES = $(shell find $(TEST_PATH) -name '*.$(SRC_EXT)' | sort -k 1nr | cut -f2-)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o) 

DEPS = $(OBJECTS:.o=.d)

COMPILE_FLAGS = -std=c11 -Wall -Wextra -Wpedantic -Werror -g
COMPILE_FLAGS_TEST_FLAGS = -Wno-gnu-zero-variadic-macro-arguments 
INCLUDES = -I include/ -I /usr/local/include -I /usr/include
TEST_LINKS = -lcheck

.PHONY: default_target
default_target: release

.PHONY: release
release: export CFLAGS := $(CFLAGS) $(COMPILE_FLAGS)
release: dirs
	@$(MAKE) all

.PHONY: dirs
dirs:
	@echo "Creating directories"
	@mkdir -p $(dir $(OBJECTS))
	@mkdir -p $(BIN_PATH)

.PHONY: clean
clean:
	@echo "Deleting $(BIN_NAME) symlink"
	@$(RM) $(BIN_NAME)
	@echo "Deleting $(TEST_BIN_NAME) symlink"
	@$(RM) $(TEST_BIN_NAME)
	@echo "Deleting directories"
	@$(RM) -r $(BUILD_PATH)
	@$(RM) -r $(BIN_PATH)

# checks the executable and symlinks to the output
.PHONY: all
all: $(BIN_PATH)/$(BIN_NAME)
	@echo "Making symlink: $(BIN_NAME) -> $<"
	@$(RM) $(BIN_NAME)
	@ln -s $(BIN_PATH)/$(BIN_NAME) $(BIN_NAME)

# Creation of the executable
$(BIN_PATH)/$(BIN_NAME): $(OBJECTS)
	@echo "Linking: $@"
	$(CC) $(OBJECTS) -o $@ -lm

.PHONY: test-objects
test-objects: dirs $(OBJECTS_TO_TEST) $(TEST_OBJECTS)
	@echo "Linking: $(BIN_PATH)/$(TEST_BIN_NAME)"
	$(CC) $(OBJECTS_TO_TEST) $(TEST_OBJECTS) -o $(BIN_PATH)/$(TEST_BIN_NAME) -lm $(TEST_LINKS) 

.PHONY: test
test: test-objects
	@echo "Making symlink: $(TEST_BIN_NAME) -> $<"
	@$(RM) $(TEST_BIN_NAME)
	@ln -s $(BIN_PATH)/$(TEST_BIN_NAME) $(TEST_BIN_NAME)
	@echo "Running tests:"
	./$(TEST_BIN_NAME)

# Add dependency files, if they exist
-include $(DEPS)

# Source file rules
# After the first compilation they will be joined with the rules from the
# dependency files to provide header dependencies
$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(SRC_EXT)
	@echo "Compiling: $< -> $@"
	$(CC) $(COMPILE_FLAGS) $(INCLUDES) -MP -MMD -c $< -o $@

# Test file rules
$(BUILD_PATH)/%.o: $(TEST_PATH)/%.$(SRC_EXT)
	@echo "Compiling: $< -> $@"
	$(CC) $(COMPILE_FLAGS) $(COMPILE_FLAGS_TEST_FLAGS) $(INCLUDES) -MP -MMD -c $< -o $@
