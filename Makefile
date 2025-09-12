CC := gcc
SRC_DIR := src
INC_DIR := include/fiesta
DOCS_DIR := docs
BUILD_DIR := lib
TESTS_DIR := tests
PYTHON_EXE := python

# Windows' mkdir doesn't have an
# equivalent to -p on Linux, so
# we have a batch script that
# implements this functionality.
ifeq ($(OS),Windows_NT)
    MKDIR := @tools/windows_mkdir.bat
	EXE_EXT := .exe
else
    MKDIR := @mkdir -p
	EXE_EXT := 
endif

override FLAGS += -I$(INC_DIR) -std=c23 -lm
OBJ_FILES := $(BUILD_DIR)/file.o $(BUILD_DIR)/str.o
TEST_EXES := $(patsubst $(TESTS_DIR)/file/%.c, $(BUILD_DIR)/%$(EXE_EXT), $(wildcard $(TESTS_DIR)/file/*.c)) \
			 $(patsubst $(TESTS_DIR)/str/%.c, $(BUILD_DIR)/%$(EXE_EXT), $(wildcard $(TESTS_DIR)/str/*.c)) \
			 $(patsubst $(TESTS_DIR)/optional/%.c, $(BUILD_DIR)/%$(EXE_EXT), $(wildcard $(TESTS_DIR)/optional/*.c))

$(BUILD_DIR)/libfiesta.a: $(OBJ_FILES)
	ar rcs -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | make_lib_dir
	$(CC) -c $< -o $@ $(FLAGS)

$(BUILD_DIR)/%$(EXE_EXT): $(TESTS_DIR)/file/%.c | make_tests_dir
	$(CC) $< -o $@ -L$(BUILD_DIR) -lfiesta -Itests $(FLAGS)

$(BUILD_DIR)/%$(EXE_EXT): $(TESTS_DIR)/str/%.c | make_tests_dir
	$(CC) $< -o $@ -L$(BUILD_DIR) -lfiesta -Itests $(FLAGS)

$(BUILD_DIR)/%$(EXE_EXT): $(TESTS_DIR)/optional/%.c | make_tests_dir
	$(CC) $< -o $@ -L$(BUILD_DIR) -lfiesta -Itests $(FLAGS)

make_lib_dir:
	$(MKDIR) $(BUILD_DIR)

make_tests_dir:
	$(MKDIR) $(TESTS_DIR)

make_docs_dir:
	$(MKDIR) $(DOCS_DIR)

all: lib docs test

dbg: FLAGS += -g
dbg: lib

opt: FLAGS += -O2
opt: lib

dbgopt: FLAGS += -Og
dbgopt: lib

lib: $(BUILD_DIR)/libfiesta.a

test: lib $(TEST_EXES)
	@$(PYTHON_EXE) tools/test.py

.PHONY: docs

docs: | make_docs_dir
	$(PYTHON_EXE) tools/make_docs.py $(DOCS_DIR)

clean:
	$(RM) $(BUILD_DIR)/libfiesta.a $(OBJ_FILES) $(TEST_EXES) $(DOCS_DIR)/index.html
